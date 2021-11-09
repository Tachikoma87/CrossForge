#ifndef TERRAIN_TILENODE_H
#define TERRAIN_TILENODE_H

#include <CForge/Graphics/SceneGraph/SGNGeometry.h>

#include "Tile.h"

using namespace std;
using namespace Eigen;

namespace Terrain {
    class TileNode: public SGNGeometry {
    public:
        struct TileData {
            Vector2f pos;
            int orientation;
            int lod;
            Tile::TileVariant variant;
        };

        TileNode(ISceneGraphNode* parent, Tile* tile, const TileData& data);

        void update(float cameraX , float cameraY);

    private:
        class TileActor: public IRenderableActor {
        public:
            TileActor(Tile* tile, Tile::TileVariant variant);

            void release() override;
            void render(RenderDevice* renderDevice) override;

        private:
            Tile* mTile;
            Tile::TileVariant mVariant;
        };

        TileData mData;
        TileActor mTileActor;
    };
}


#endif
