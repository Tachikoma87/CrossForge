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
        TerrainMap();
        ~TerrainMap();

        void spawnClipmapTiles(SGNTransformation* rootTransform);

        void update(float cameraX , float cameraY);
        void render(RenderDevice* renderDevice, ClipMap::TileVariant variant);

        GLTexture2D *getTexture() const;

    private:
        void initShader();

        HeightMap* mHeightMap;
        ClipMap* mClipMap;
        GLShader* mShader;
        vector<TileNode*> mTileNodes;
    };
}

#endif
