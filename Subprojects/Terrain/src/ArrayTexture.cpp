#include "ArrayTexture.h"

namespace Terrain {

    ArrayTexture::ArrayTexture(int32_t count, int32_t size) : mCount(count), mSize(size) {
        glGenTextures(1, &mTextureArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArray);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, log2(size), GL_RGB8, mSize, mSize, mCount);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // geilo
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    ArrayTexture::~ArrayTexture() {
        glDeleteTextures(1, &mTextureArray);
    }

    void ArrayTexture::bind() {
        glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArray);
    }

    void ArrayTexture::setTexture(T2DImage<uint8_t>* image, int32_t index) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, mSize, mSize, 1, GL_RGB, GL_UNSIGNED_BYTE, image->data());
    }

    void ArrayTexture::generateMipmap() {
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
}