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

        void generate(HeightMapConfig config);

        void bindTexture();
    private:
        void initShader();

        GLTexture2D* mTexture;
        GLShader* mShader;
    };
}

#endif
