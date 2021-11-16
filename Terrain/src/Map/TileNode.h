#ifndef TERRAIN_TILENODE_H
#define TERRAIN_TILENODE_H

#include <CForge/Graphics/SceneGraph/SGNGeometry.h>
#include "TileActor.h"

using namespace std;
using namespace Eigen;

class TerrainMap;

namespace Terrain {
    class TileNode: public SGNGeometry {
    public:
        struct TileData {
            Vector2f pos;
            int orientation;
            int lod;
            ClipMap::TileVariant variant;
        };

        TileNode(ISceneGraphNode* parent, TerrainMap* map, TileData data);

        void update(float cameraX , float cameraY);

    private:
        TileData mData;
        TileActor mTileActor;
    };
}

#endif
