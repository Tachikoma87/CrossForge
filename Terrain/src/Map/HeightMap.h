#ifndef TERRAIN_HEIGHTMAP_H
#define TERRAIN_HEIGHTMAP_H

#include <CForge/Graphics/GLTexture2D.h>
#include <CForge/Graphics/Shader/GLShader.h>

using namespace CForge;

namespace Terrain {
    class HeightMap {
    public:
        struct HeightMapConfig {
            int width;
            int height;
        };

        HeightMap();
        ~HeightMap();

        void generate();

        GLTexture2D *getTexture() const;
        void setConfig(HeightMapConfig config);
    private:
        void initShader();

        HeightMapConfig mConfig;
        GLTexture2D* mTexture;
        GLShader* mShader;
    };
}

#endif
