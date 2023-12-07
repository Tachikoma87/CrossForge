#include "TileActor.h"
#include "TerrainMap.h"

namespace Terrain {
    TileActor::TileActor(TerrainMap* map, ClipMap::TileVariant variant)
        : IRenderableActor("TileActor", ATYPE_STATIC), mMap(map), mVariant(variant) {}

    void TileActor::release() {
        delete this;
    }

    void TileActor::render(RenderDevice* renderDevice) {
        mMap->render(renderDevice, mVariant);
    }
}