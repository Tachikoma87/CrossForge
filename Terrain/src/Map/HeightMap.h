#pragma once

#include <CForge/Graphics/GLTexture2D.h>
#include <CForge/Graphics/Shader/GLShader.h>

using namespace CForge;
using namespace Eigen;

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
            float mapHeight;
            NoiseConfig noiseConfig;
        };

        HeightMap();

        // Todo: destroy old textures once all are properly stored in the texture manager
        void generate(HeightMapConfig config);
        const HeightMapConfig &getConfig();
        void setConfig(HeightMapConfig config);

        float getHeightAt(float x, float y);
        Vector3f getNormalAt(float x, float y);

        void erode(int32_t count);
        void updateHeights();

        void setTexture(GLTexture2D* texture);
        void bindTexture();
    private:
        void bindNoiseData(NoiseConfig config);
        void initShader();

        HeightMapConfig mConfig;

        GLfloat* mHeights;
        GLTexture2D* mTexture;
        GLShader* mHeightMapShader;
        GLShader* mErosionShader;
    };
}
