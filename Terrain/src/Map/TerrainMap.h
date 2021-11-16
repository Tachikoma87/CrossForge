#ifndef TERRAIN_TERRAINMAP_H
#define TERRAIN_TERRAINMAP_H

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

        void update(float cameraX , float cameraY); // Todo: these should live somewhere else

        void generateClipMap(ClipMap::ClipMapConfig clipMapConfig);
        void generateHeightMap(HeightMap::HeightMapConfig heightMapConfig);

        void render(RenderDevice* renderDevice, ClipMap::TileVariant variant);
        void bindTexture();
    private:
        void clear();
        void initShader();
        void generateClipmapTiles(); // Todo: these should live somewhere else

        ClipMap::ClipMapConfig mClipMapConfig;
        HeightMap::HeightMapConfig mHeightMapConfig;

        ClipMap mClipMap;
        HeightMap mHeightMap;

        GLShader* mShader;
        SGNTransformation* mRootTransform;

        vector<TileNode*> mTileNodes; // Todo: these should live somewhere else
    };
}

#endif
