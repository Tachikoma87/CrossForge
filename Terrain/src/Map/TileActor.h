#pragma once

#include <CForge/Graphics/Actors/IRenderableActor.h>
#include "ClipMap.h"

using namespace CForge;

namespace Terrain {
    class TerrainMap;

    class TileActor: public IRenderableActor {
    public:
        TileActor(TerrainMap* map, ClipMap::TileVariant variant);

        void release() override;
        void render(RenderDevice* renderDevice) override;
    private:
        TerrainMap* mMap;
        ClipMap::TileVariant mVariant;
    };
}
