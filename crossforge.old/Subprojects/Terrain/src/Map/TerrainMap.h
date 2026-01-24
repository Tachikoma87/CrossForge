#pragma once

#include "../../../CForge/Graphics/RenderDevice.h"
#include "../../../CForge/Graphics/SceneGraph/SGNTransformation.h"
#include "HeightMap.h"
#include "../ArrayTexture.h"
#include "TileNode.h"

#include "../../../Prototypes/UBOInstancedData.h"


using namespace CForge;
using namespace Eigen;

namespace Terrain {
    class TerrainMap {
    public:
        TerrainMap(SGNTransformation *rootTransform);
        ~TerrainMap();

        void update(float cameraX , float cameraY);
        void erode();
        void updateHeights();

        void generateClipMap(ClipMap::ClipMapConfig clipMapConfig);
        void generateHeightMap(HeightMap::HeightMapConfig heightMapConfig);

        void heightMapFromTexture(GLTexture2D* texture, float mapHeight);

        Vector2f getMapSize();
        float getHeightAt(float x, float y);
        Vector3f getNormalAt(float x, float y);

        void render(RenderDevice* renderDevice, ClipMap::TileVariant variant);
		void renderMap(RenderDevice* pRDev);
        void bindTexture();
		void prepareRender(RenderDevice* renderDevice);
	private:
        void clear();
        void initShader();
        void generateClipMapTiles();
		void initShadowPassShader();

        ClipMap mClipMap;
        HeightMap mHeightMap;

        ArrayTexture mTextures;

        GLShader* mShader;

        SGNTransformation* mRootTransform;
        vector<TileNode*> mTileNodes;
		vector<TileActor> m_TileActors;
		UBOInstancedData mUBOinstances;
		uint32_t m_CurrentInstanceAmount = 0;
		GLShader* m_pShadowPassShaderMap;
    };
}
