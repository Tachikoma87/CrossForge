#include <glad/glad.h>
#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/Graphics/STextureManager.h>
#include "HeightMap.h"

using namespace std;

namespace Terrain {
    HeightMap::HeightMap() : mTexture(nullptr) {
        initShader();
        mConfig = {
            1024 * 8,
            1024 * 8,
        };
    }

    HeightMap::~HeightMap() {
        delete mTexture;
    }

    void HeightMap::generate() {
        if (mTexture) {
            delete mTexture;
        }

        GLuint textureHandle;
        glGenTextures(1, &textureHandle);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mConfig.width, mConfig.height, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindImageTexture(0, textureHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        mTexture = STextureManager::fromHandle(textureHandle);

        mShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindImageTexture(0, mTexture->handle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA);
        glDispatchCompute(mConfig.width, mConfig.height, 1);

        // make sure writing to image has finished before read
        // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    GLTexture2D *HeightMap::getTexture() const {
        return mTexture;
    }

    void HeightMap::setConfig(Terrain::HeightMap::HeightMapConfig config) {
        mConfig = config;
    }

    void HeightMap::initShader() {
        vector<ShaderCode*> csSources;
        string errorLog;

        SShaderManager* shaderManager = SShaderManager::instance();
        csSources.push_back(shaderManager->createShaderCode("Shader/HeightMapShader.comp", "430",
                                                            0, "", ""));
        mShader = shaderManager->buildComputeShader(&csSources, &errorLog);
        shaderManager->release();
    }
}
