#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/AssetIO/SAssetIO.h>
#include <Terrain/src/ArrayTexture.h>
#include <CForge/Core/SLogger.h>
#include "TerrainMap.h"

namespace Terrain {
    TerrainMap::TerrainMap(SGNTransformation *rootTransform)
        : mRootTransform(rootTransform), mTextures(6, 1024) {
        initShader();

		mUBOinstances.init();
		
		for (uint32_t i = 0; i < ClipMap::TileVariant::count; i++) {
			TileActor actor = TileActor(this, ClipMap::TileVariant(i));
			m_TileActors.push_back(actor);
		}
		
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

		initShadowPassShader();
    }

    TerrainMap::~TerrainMap() {
        clear();
    }

    void TerrainMap::update(float cameraX, float cameraY) {
        for (uint32_t i = 0; i < mTileNodes.size(); i++) {
			auto node = mTileNodes[i];
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

        HeightMap::HeightMapConfig C;
        HeightMap::NoiseConfig NC;

        NC.lacunarity = 0;
        NC.octaves = 0;
        NC.persistence = 0;
        NC.scale = 0;
        NC.seed = 0;

        C.width = 2048 / 2;
        C.height = 2048 / 2;
        C.mapHeight = mapHeight;
        C.noiseConfig = NC;
        mHeightMap.setConfig(C);

        //mHeightMap.setConfig({.width = 2048/2, .height = 2048/2, .mapHeight = mapHeight, .noiseConfig = {0, 0, 0, 0, 0}});


    }

    void TerrainMap::render(RenderDevice *renderDevice, Terrain::ClipMap::TileVariant variant) {
        mClipMap.bindTile(variant);
        //glDrawElements(GL_TRIANGLES, mClipMap.getIndexCount(variant), GL_UNSIGNED_INT, nullptr);
		glDrawElementsInstanced(GL_TRIANGLES, mClipMap.getIndexCount(variant), GL_UNSIGNED_INT, nullptr, m_CurrentInstanceAmount);
        mClipMap.unbindTile(variant);
    }
	
	void TerrainMap::prepareRender(RenderDevice* renderDevice) {
		renderDevice->activeShader(mShader);
		glActiveTexture(GL_TEXTURE0);
		mHeightMap.bindTexture();

		glActiveTexture(GL_TEXTURE1);
		mTextures.bind();

		int layerCount = 6;
		vector<GLfloat> layerHeights{ 0.501, 0.52, 0.56, 0.65, 0.78 };
		vector<GLfloat> blendValues{ 0.001, 0.03, 0.1, 0.1, 0.03 };

		glUniform1i(mShader->uniformLocation("LayerCount"), layerCount);
		glUniform1fv(mShader->uniformLocation("LayerHeights"), layerHeights.size(), layerHeights.data());
		glUniform1fv(mShader->uniformLocation("BlendValues"), blendValues.size(), blendValues.data());

		glUniform1i(mShader->uniformLocation("HeightMap"), 0);
		glUniform1i(mShader->uniformLocation("Textures"), 1);
		glUniform1f(mShader->uniformLocation("MapHeight"), mHeightMap.getConfig().mapHeight);
	}
	
	void TerrainMap::renderMap(RenderDevice* pRDev) {
		if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
			pRDev->activeShader(m_pShadowPassShaderMap);
			glActiveTexture(GL_TEXTURE0);
			mHeightMap.bindTexture();
			
			glUniform1i(m_pShadowPassShaderMap->uniformLocation("HeightMap"), 0);
			glUniform1f(m_pShadowPassShaderMap->uniformLocation("MapHeight"), mHeightMap.getConfig().mapHeight);
			uint32_t BindingPoint = m_pShadowPassShaderMap->uboBindingPoint(GLShader::DEFAULTUBO_INSTANCE);
			if (GL_INVALID_INDEX != BindingPoint) {
				mUBOinstances.bind(BindingPoint);
			}
			glDisable(GL_CULL_FACE);
		}
		else {
			prepareRender(pRDev);
			uint32_t BindingPoint = mShader->uboBindingPoint(GLShader::DEFAULTUBO_INSTANCE);
			if (GL_INVALID_INDEX != BindingPoint) {
				mUBOinstances.bind(BindingPoint);
			}
		}
		for (uint32_t i = 0; i < m_TileActors.size(); i++) {
			uint32_t instanceIndex = 0;
			for (uint32_t j = 0; j < mTileNodes.size(); j++) {
				TileNode::TileData data = mTileNodes[j]->getTileData();
				if (data.variant == i) {
					Eigen::Matrix4f mat = (mTileNodes[j]->m_trans);
					mUBOinstances.setInstance(&mat, instanceIndex);
					instanceIndex++;
				}
			}
			if (instanceIndex > 500) {
				CForgeExcept("too many instances");
				exit(-1);
			}
			m_CurrentInstanceAmount = instanceIndex;
			m_TileActors[i].render(pRDev);
		}
		if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
			pRDev->activeShader(pRDev->shadowPassShader()); //TODO dirty fix
			glEnable(GL_CULL_FACE);
		}
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
                                            0, "");
        ShaderCode* fragmentShader =
            shaderManager->createShaderCode("Shader/Map.frag", "330 core",
                                            0, "");

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
        mTileNodes.push_back(new TileNode(data));

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
                        mTileNodes.push_back(new TileNode(data));
                    }
                }

                data = {linePositions[y] * scale, y, lod, ClipMap::Line};
                mTileNodes.push_back(new TileNode(data));
            }

            data = {Vector2f(scale, scale), 0, lod, ClipMap::Trim};
            mTileNodes.push_back(new TileNode(data));
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

	
	void TerrainMap::initShadowPassShader() {
		SShaderManager* pSMan = SShaderManager::instance();
		string ErrorLog;

		std::vector<ShaderCode*> VSSources;
		std::vector<ShaderCode*> FSSources;
		ShaderCode* pSC = nullptr;

		pSC = pSMan->createShaderCode("Shader/MapShadow.vert", "330 core", ShaderCode::CONF_LIGHTING, "highp");
		VSSources.push_back(pSC);
		pSC = pSMan->createShaderCode("Shader/ShadowPassShader.frag", "330 core", 0, "highp");
		FSSources.push_back(pSC);
		m_pShadowPassShaderMap = pSMan->buildShader(&VSSources, &FSSources, &ErrorLog);
		if (nullptr == m_pShadowPassShaderMap || !ErrorLog.empty()) {
			SLogger::log(ErrorLog);
			throw CForgeExcept("Building shadow pass shader failed. See log for details.");
		}

		pSMan->release();
	}
}
