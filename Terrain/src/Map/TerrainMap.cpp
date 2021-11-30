#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/AssetIO/SAssetIO.h>
#include <Terrain/src/ArrayTexture.h>
#include <CForge/Core/SLogger.h>
#include "TerrainMap.h"

namespace Terrain {
    TerrainMap::TerrainMap(SGNTransformation *rootTransform)
        : mRootTransform(rootTransform), mMapScale(1.0f), mMapHeight(2000.0f), mTextures(7, 1024) {
        initShader();

        vector<string> files;
        files.push_back("Assets/water.jpg");
        files.push_back("Assets/sand.jpg");
        files.push_back("Assets/ground.jpg");
        files.push_back("Assets/grass.jpg");
        files.push_back("Assets/grass_rock2.jpg");
        files.push_back("Assets/rock2.jpg");
        files.push_back("Assets/snow.jpg");

        for (int i = 0; i < files.size(); i++) {
            string file = files[i];

            if (!File::exists(file)) throw CForgeExcept("Image file " + file + " could not be found!");

            T2DImage<uint8_t> image;

            try {
                SAssetIO::load(file, &image);
            }
            catch (CrossForgeException& e) {
                SLogger::logException(e);
            }

            mTextures.setTexture(&image, i);
        }
    }

    TerrainMap::~TerrainMap() {
        clear();
    }

    void TerrainMap::update(float cameraX, float cameraY) {
        cameraX /= mMapScale;
        cameraY /= mMapScale;

        for (auto node : mTileNodes) {
            node->update(cameraX, cameraY);
        }
    }

    void TerrainMap::generateClipMap(ClipMap::ClipMapConfig clipMapConfig) {
        mClipMapConfig = clipMapConfig;
        mClipMap.generate(mClipMapConfig);
        generateClipMapTiles();
    }

    void TerrainMap::generateHeightMap(HeightMap::HeightMapConfig heightMapConfig) {
        mHeightMapConfig = heightMapConfig;
        mHeightMap.generate(mHeightMapConfig);
    }

    void TerrainMap::heightMapFromTexture(GLTexture2D *texture) {
        mHeightMap.setTexture(texture);
    }

    void TerrainMap::setMapHeight(float mapHeight) {
        mMapHeight = mapHeight;
    }

    void TerrainMap::setMapScale(float mapScale) {
        mMapScale = mapScale;
    }

    void TerrainMap::render(RenderDevice *renderDevice, Terrain::ClipMap::TileVariant variant) {
        mClipMap.bindTile(variant);
        renderDevice->activeShader(mShader);
        glActiveTexture(GL_TEXTURE0);
        mHeightMap.bindTexture();

        glActiveTexture(GL_TEXTURE1);
        mTextures.bind();

        glUniform1i(mShader->uniformLocation("HeightMap"), 0);
        glUniform1i(mShader->uniformLocation("Textures"), 1);
        glUniform1f(mShader->uniformLocation("MapScale"), mMapScale);
        glUniform1f(mShader->uniformLocation("MapHeight"), mMapHeight);

        glDrawElements(GL_TRIANGLES, mClipMap.getIndexCount(variant), GL_UNSIGNED_INT, nullptr);
    }

    void TerrainMap::bindTexture() {
        mHeightMap.bindTexture();
    }

    void TerrainMap::clear() {
        for (auto node : mTileNodes) {
            delete node;
        }

        mTileNodes.clear();
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

    void TerrainMap::generateClipMapTiles() {
        clear();

        const tuple<ClipMap::TileVariant, uint32_t> TILE_ALIGNMENTS[4][4] = {
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

        float sideLength = static_cast<float>(mClipMapConfig.sideLength);
        float lineOffset = sideLength * 1.5f;
        Vector2f linePositions[4] = {
            Vector2f(-lineOffset, 1.0f),
            Vector2f(1.0f, lineOffset + 2.0f),
            Vector2f(lineOffset + 2.0f, 1.0f),
            Vector2f(1.0f, -lineOffset),
        };

        TileNode::TileData data = {Vector2f(2, 2), 0, 2, ClipMap::Cross};
        mTileNodes.push_back(new TileNode(mRootTransform, this, data));

        for (uint32_t level = 0; level < mClipMapConfig.levelCount; level++) {
            uint32_t lod = 2 << level;
            float scale = static_cast<float>(lod);

            for (uint32_t y = 0; y < 4; y++) {
                for (uint32_t x = 0; x < 4; x++) {
                    if (level == 0 || x == 0 || x == 3 || y == 0 || y == 3) {
                        auto[variant, orientation] = TILE_ALIGNMENTS[y][x];

                        Vector2f pos = Vector2f((static_cast<float>(x) - 1.5f) * sideLength * scale,
                                            (static_cast<float>(y) - 1.5f) * sideLength * scale) +
                                   Vector2f(x < 2 ? 0 : 2, y < 2 ? 0 : 2) * lod;

                        data = {pos, orientation, lod, variant};
                        mTileNodes.push_back(new TileNode(mRootTransform, this, data));
                    }
                }

                data = {linePositions[y] * scale, y, lod, ClipMap::Line};
                mTileNodes.push_back(new TileNode(mRootTransform, this, data));
            }

            data = {Vector2f(scale, scale), 0, lod, ClipMap::Trim};
            mTileNodes.push_back(new TileNode(mRootTransform, this, data));
        }
    }
}
