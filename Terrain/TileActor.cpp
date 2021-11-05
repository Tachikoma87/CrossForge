#include "CForge/Graphics/RenderDevice.h"

#include "TileActor.h"

namespace Terrain {
    TileActor::TileActor() : IRenderableActor("MapActor", ATYPE_STATIC), mTile(nullptr), mVariant(Tile::Normal) {}

    void TileActor::init(Tile* tile, Tile::TileVariant variant) {
        mTile = tile;
        mVariant = variant;
    }

    void TileActor::release() {
        delete this;
    }

    void TileActor::render(RenderDevice* renderDevice) {
        mTile->render(renderDevice, mVariant);
    }
}
