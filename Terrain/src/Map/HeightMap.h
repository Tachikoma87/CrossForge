#pragma once

#include <CForge/Graphics/GLTexture2D.h>
#include <CForge/Graphics/Shader/GLShader.h>

using namespace CForge;

namespace Terrain {
    class HeightMap {
    public:
        // Todo: refactor to class once more noise parameters are supported
        struct NoiseConfig {
            uint32_t seed;
            float scale;
            uint32_t octaves;
            float persistence;
            float lacunarity;
        };

        struct HeightMapConfig {
            uint32_t width;
            uint32_t height;
            NoiseConfig noiseConfig;
        };

        HeightMap();

        // Todo: destroy old textures once all are properly stored in the texture manager
        void generate(HeightMapConfig config);
        void setTexture(GLTexture2D* texture);

        void bindTexture();
    private:
        void bindNoiseData(NoiseConfig config);
        void initShader();

        GLTexture2D* mTexture;
        GLShader* mShader;
    };
}
