#pragma once

#include <CForge/Graphics/RenderDevice.h>
#include <CForge/Graphics/SceneGraph/SGNTransformation.h>
#include "HeightMap.h"
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

        void render(RenderDevice* renderDevice, ClipMap::TileVariant variant);
        void bindTexture();
    private:
        void clear();
        void initShader();
        void generateClipMapTiles();

        ClipMap::ClipMapConfig mClipMapConfig;
        HeightMap::HeightMapConfig mHeightMapConfig;

        ClipMap mClipMap;
        HeightMap mHeightMap;

        GLShader* mShader;

        SGNTransformation* mRootTransform;
        vector<TileNode*> mTileNodes;
    };
}
