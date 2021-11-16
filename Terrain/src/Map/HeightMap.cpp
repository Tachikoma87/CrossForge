#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/Graphics/STextureManager.h>
#include <glad/glad.h>
#include "HeightMap.h"

using namespace std;

namespace Terrain {
    HeightMap::HeightMap() : mTexture() {
        initShader();
    }

    HeightMap::~HeightMap() {
        delete mTexture;
    }

    void HeightMap::generate(HeightMapConfig config) {
        delete mTexture;

        GLuint textureHandle;
        glGenTextures(1, &textureHandle);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, config.width, config.height, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindImageTexture(0, textureHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        mTexture = STextureManager::fromHandle(textureHandle);

        mShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindImageTexture(0, mTexture->handle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA);
        glDispatchCompute(config.width, config.height, 1);
    }

    void HeightMap::bindTexture() {
        mTexture->bind();
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
