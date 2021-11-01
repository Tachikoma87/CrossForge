#include "CForge/Graphics/RenderDevice.h"

#include <glad/glad.h>

#include "TileActor.h"
#include "Tile.h"

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
