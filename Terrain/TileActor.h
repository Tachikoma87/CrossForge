#ifndef MAP_ACTOR_H
#define MAP_ACTOR_H

#include "Tile.h"

using namespace std;

namespace Terrain {
    class TileActor: public IRenderableActor {
    public:
        TileActor();

        void init(Tile* tile, Tile::TileVariant variant);

        void release() override;
        void render(RenderDevice* renderDevice) override;

    private:
        Tile* mTile{};
        Tile::TileVariant mVariant;
    };
}

#endif
