#pragma once

#include <CForge/Graphics/RenderDevice.h>
#include <CForge/Graphics/SceneGraph/SGNTransformation.h>
#include "HeightMap.h"
#include "../ArrayTexture.h"
#include "TileNode.h"

using namespace CForge;

namespace Terrain {
    class TerrainMap {
    public:
        TerrainMap(SGNTransformation *rootTransform);
        ~TerrainMap();

        void update(float cameraX , float cameraY);

        void generateClipMap(ClipMap::ClipMapConfig clipMapConfig);
        void generateHeightMap(HeightMap::HeightMapConfig heightMapConfig);

        void heightMapFromTexture(GLTexture2D* texture);

        void setMapHeight(float mapHeight);
        void setMapScale(float mapScale);

        void render(RenderDevice* renderDevice, ClipMap::TileVariant variant);
        void bindTexture();
    private:
        void clear();
        void initShader();
        void generateClipMapTiles();

        ClipMap::ClipMapConfig mClipMapConfig;
        HeightMap::HeightMapConfig mHeightMapConfig; // Todo: only store here if needed

        // Todo: group those once more parameters are introduced
        float mMapScale;
        float mMapHeight;

        ClipMap mClipMap;
        HeightMap mHeightMap;

        ArrayTexture mTextures;

        GLShader* mShader;

        SGNTransformation* mRootTransform;
        vector<TileNode*> mTileNodes;
    };
}
