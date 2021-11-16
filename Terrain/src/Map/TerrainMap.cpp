#include <CForge/Graphics/Shader/SShaderManager.h>
#include "TerrainMap.h"

namespace Terrain {
    TerrainMap::TerrainMap() {
        initShader();
        mHeightMap = new HeightMap();
        mHeightMap->generate();
        mClipMap = new ClipMap(64);
    }

    TerrainMap::~TerrainMap() {
        delete mHeightMap;
        delete mClipMap;

        for (auto node : mTileNodes) {
            delete node;
        }
    }

    void TerrainMap::spawnClipmapTiles(SGNTransformation *rootTransform) {
        const uint32_t LOD_LEVELS = 6; // Todo: move into tile
        const tuple<ClipMap::TileVariant, int> TILE_ALIGNMENTS[4][4] = {
            {
                {ClipMap::Corner, 0},
                {ClipMap::Edge, 3},
                {ClipMap::Edge, 3},
                {ClipMap::Corner, 3},
            },
            {
                {ClipMap::Edge, 0},
                {ClipMap::Normal, 0},
                {ClipMap::Normal, 0},
                {ClipMap::Edge, 2},
            },
            {
                {ClipMap::Edge, 0},
                {ClipMap::Normal, 0},
                {ClipMap::Normal, 0},
                {ClipMap::Edge, 2},
            },
            {
                {ClipMap::Corner, 1},
                {ClipMap::Edge, 1},
                {ClipMap::Edge, 1},
                {ClipMap::Corner, 2},
            },
        };

        float sideLength = static_cast<float>(mClipMap->sideLength());
        float lineOffset = sideLength * 1.5f;
        Vector2f linePositions[4] = {
            Vector2f(-lineOffset, 1.0f),
            Vector2f(1.0f, lineOffset + 2.0f),
            Vector2f(lineOffset + 2.0f, 1.0f),
            Vector2f(1.0f, -lineOffset),
        };

        TileNode::TileData data = {Vector2f(2, 2), 0, 2, ClipMap::Cross};
        mTileNodes.push_back(new TileNode(rootTransform, this, data));

        for (int level = 0; level < LOD_LEVELS; level++) {
            int lod = 2 << level;
            float scale = static_cast<float>(lod);

            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    if (level == 0 || x == 0 || x == 3 || y == 0 || y == 3) {
                        auto[variant, orientation] = TILE_ALIGNMENTS[y][x];

                        auto pos = Vector2f((static_cast<float>(x) - 1.5f) * sideLength * scale,
                                            (static_cast<float>(y) - 1.5f) * sideLength * scale) +
                                   Vector2f(x < 2 ? 0 : 2, y < 2 ? 0 : 2) * lod;

                        data = {pos, orientation, lod, variant};
                        mTileNodes.push_back(new TileNode(rootTransform, this, data));
                    }
                }

                data = {linePositions[y] * scale, y, lod, ClipMap::Line};
                mTileNodes.push_back(new TileNode(rootTransform, this, data));
            }

            data = {Vector2f(scale, scale), 0, lod, ClipMap::Trim};
            mTileNodes.push_back(new TileNode(rootTransform, this, data));
        }
    }

    void TerrainMap::update(float cameraX, float cameraY) {
        for (auto node : mTileNodes) {
            node->update(cameraX, cameraY);
        }
    }

    void TerrainMap::render(RenderDevice *renderDevice, Terrain::ClipMap::TileVariant variant) {
        mClipMap->bindTile(variant);
        renderDevice->activeShader(mShader);
        glActiveTexture(GL_TEXTURE0);
        mHeightMap->getTexture()->bind();
        glUniform1i(mShader->uniformLocation("HeightMap"), 0);
        glDrawElements(GL_TRIANGLES, mClipMap->getIndexCount(variant), GL_UNSIGNED_INT, nullptr);
    }

    GLTexture2D *TerrainMap::getTexture() const {
        return mHeightMap->getTexture();
    }

    void TerrainMap::initShader() {
        SShaderManager* shaderManager = SShaderManager::instance();

        vector<ShaderCode*> vsSources;
        vector<ShaderCode*> fsSources;
        string errorLog;

        ShaderCode* vertexShader =
            shaderManager->createShaderCode("Shader/MapShader.vert", "330 core",
                                            0, "", "");
        ShaderCode* fragmentShader =
            shaderManager->createShaderCode("Shader/MapShader.frag", "330 core",
                                            0, "", "");

        vsSources.push_back(vertexShader);
        fsSources.push_back(fragmentShader);

        mShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);

        shaderManager->release();
    }
}
