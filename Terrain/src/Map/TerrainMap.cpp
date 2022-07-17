#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/AssetIO/SAssetIO.h>
#include <Terrain/src/ArrayTexture.h>
#include <CForge/Core/SLogger.h>
#include "TerrainMap.h"

namespace Terrain {
    TerrainMap::TerrainMap(SGNTransformation *rootTransform)
        : mRootTransform(rootTransform), mTextures(6, 1024) {
        initShader();

        vector<string> files;
        files.push_back("Assets/water.jpg");
        files.push_back("Assets/sand.jpg");
        files.push_back("Assets/grass.jpg");
        files.push_back("Assets/grass_rock.jpg");
        files.push_back("Assets/rock.jpg");
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

        mTextures.generateMipmap();
    }

    TerrainMap::~TerrainMap() {
        clear();
    }

    void TerrainMap::update(float cameraX, float cameraY) {
        for (auto node : mTileNodes) {
            node->update(cameraX, cameraY);
        }
    }

    void TerrainMap::erode() {
        mHeightMap.erode(10);
    }

    void TerrainMap::generateClipMap(ClipMap::ClipMapConfig clipMapConfig) {
        mClipMap.generate(clipMapConfig);
        generateClipMapTiles();
    }

    void TerrainMap::generateHeightMap(HeightMap::HeightMapConfig heightMapConfig) {
        mHeightMap.generate(heightMapConfig);
    }

    void TerrainMap::heightMapFromTexture(GLTexture2D *texture, float mapHeight) {
        mHeightMap.setTexture(texture);
        mHeightMap.setConfig({.width = 2048/2, .height = 2048/2, .mapHeight = mapHeight, .noiseConfig = {0, 0, 0, 0, 0}});
    }

    void TerrainMap::render(RenderDevice *renderDevice, Terrain::ClipMap::TileVariant variant) {
        mClipMap.bindTile(variant);
        renderDevice->activeShader(mShader);
        glActiveTexture(GL_TEXTURE0);
        mHeightMap.bindTexture();

        glActiveTexture(GL_TEXTURE1);
        mTextures.bind();

        int layerCount = 6;
        vector<GLfloat> layerHeights {0.0, 0.52, 0.56, 0.65, 0.78};
        vector<GLfloat> blendValues {0.001, 0.03, 0.1, 0.1, 0.03};

        glUniform1i(mShader->uniformLocation("LayerCount"), layerCount);
        glUniform1fv(mShader->uniformLocation("LayerHeights"), layerHeights.size(), layerHeights.data());
        glUniform1fv(mShader->uniformLocation("BlendValues"), blendValues.size(), blendValues.data());

        glUniform1i(mShader->uniformLocation("HeightMap"), 0);
        glUniform1i(mShader->uniformLocation("Textures"), 1);
        glUniform1f(mShader->uniformLocation("MapHeight"), mHeightMap.getConfig().mapHeight);

        glDrawElements(GL_TRIANGLES, mClipMap.getIndexCount(variant), GL_UNSIGNED_INT, nullptr);
        mClipMap.unbindTile(variant);
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
            shaderManager->createShaderCode("Shader/Map.vert", "330 core",
                                            0, "", "");
        ShaderCode* fragmentShader =
            shaderManager->createShaderCode("Shader/Map.frag", "330 core",
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

        float sideLength = static_cast<float>(mClipMap.getConfig().sideLength);
        float lineOffset = sideLength * 1.5f;
        Vector2f linePositions[4] = {
            Vector2f(-lineOffset, 1.0f),
            Vector2f(1.0f, lineOffset + 2.0f),
            Vector2f(lineOffset + 2.0f, 1.0f),
            Vector2f(1.0f, -lineOffset),
        };

        TileNode::TileData data = {Vector2f(2, 2), 0, 2, ClipMap::Cross};
        mTileNodes.push_back(new TileNode(mRootTransform, this, data));

        for (uint32_t level = 0; level < mClipMap.getConfig().levelCount; level++) {
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

    float TerrainMap::getHeightAt(float x, float y) {
        return mHeightMap.getHeightAt((x + mHeightMap.getConfig().width / 2),
                                      (y + mHeightMap.getConfig().height / 2));
    }

    Vector3f TerrainMap::getNormalAt(float x, float y) {
        return mHeightMap.getNormalAt((x + mHeightMap.getConfig().width / 2),
                                      (y + mHeightMap.getConfig().height / 2));
    }

    Vector2f TerrainMap::getMapSize() {
        return Vector2f(mHeightMap.getConfig().width, mHeightMap.getConfig().height);
    }

    void TerrainMap::updateHeights() {
        mHeightMap.updateHeights();
    }
}
