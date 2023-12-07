#pragma once

#include <CForge/Graphics/OpenGLHeader.h>

using namespace CForge;
using namespace std;

namespace Terrain {
    class ArrayTexture {
    public:
        ArrayTexture(int32_t count, int32_t size);
        ~ArrayTexture();

        void setTexture(T2DImage<uint8_t>* image, int32_t index);
        void bind();
        void generateMipmap();
    private:
        int32_t mSize;
        int32_t mCount;
        GLuint mTextureArray;
    };
}
