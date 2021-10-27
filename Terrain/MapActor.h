#ifndef MAP_ACTOR_H
#define MAP_ACTOR_H

#include "../CForge/Graphics/Actors/IRenderableActor.h"

using namespace CForge;
using namespace Eigen;
using namespace std;

namespace Terrain {
    class MapActor: public IRenderableActor {
    public:
        MapActor();
        ~MapActor() override;

        void init();
        void clear();
        void release() override;

        void render(RenderDevice* renderDevice) override;

    protected:
        void setBufferData() override;

    private:
        GLShader* m_pShader;

        void setShader();
    };
}

#endif
