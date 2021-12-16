#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/Graphics/STextureManager.h>
#include <glad/glad.h>
#include "HeightMap.h"

using namespace std;

float randf() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

namespace Terrain {
    HeightMap::HeightMap() : mTexture() {
        initShader();
    }

    void HeightMap::generate(HeightMapConfig config) {
//        GLint internalFormat = GL_R16UI;
//        GLint format = GL_RED_INTEGER;
//        GLint dataType = GL_UNSIGNED_SHORT;

        mMapHeight = config.mapHeight;

        GLint internalFormat = GL_R32F;
        GLint format = GL_RED;
        GLint dataType = GL_FLOAT;

        GLuint textureHandle;
        glGenTextures(1, &textureHandle);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, config.width, config.height, 0, format, dataType, NULL);
        glBindImageTexture(0, textureHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, internalFormat);

        mTexture = STextureManager::fromHandle(textureHandle);

        mHeightMapShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindImageTexture(0, mTexture->handle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, format);
        bindNoiseData(config.noiseConfig);
        glDispatchCompute(config.width, config.height, 1);
    }

    void HeightMap::setTexture(GLTexture2D* texture) {
        mTexture = texture;
    }

    void HeightMap::bindTexture() {
        mTexture->bind();
    }

    void HeightMap::bindNoiseData(NoiseConfig config) {
        srand(config.seed);

        glUniform1f(mHeightMapShader->uniformLocation("Noise.scale"), config.scale);
        glUniform1ui(mHeightMapShader->uniformLocation("Noise.octaves"), config.octaves);
        glUniform1f(mHeightMapShader->uniformLocation("Noise.persistence"), config.persistence);
        glUniform1f(mHeightMapShader->uniformLocation("Noise.lacunarity"), config.lacunarity);

        for (int i = 0; i < config.octaves; ++i) {
            glUniform2f(mHeightMapShader->uniformLocation("Noise.offsets[" + to_string(i) + "]"), randf(), randf());
        }
    }

    void HeightMap::initShader() {
        vector<ShaderCode*> csSources;
        string errorLog;

        SShaderManager* shaderManager = SShaderManager::instance();
        csSources.push_back(shaderManager->createShaderCode("Shader/HeightMap.comp", "430",
                                                            0, "", ""));
        mHeightMapShader = shaderManager->buildComputeShader(&csSources, &errorLog);

        csSources.clear();
        csSources.push_back(shaderManager->createShaderCode("Shader/Erosion.comp", "430",
                                                            0, "", ""));
        mErosionShader = shaderManager->buildComputeShader(&csSources, &errorLog);

        shaderManager->release();
    }

    void HeightMap::erode(int32_t count) {
        int radius = 8;
        GLint borderSize = radius;

        vector<GLfloat> brushWeights;
        vector<GLint> brushOffsets;

        float weightSum = 0;
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                float sqrDst = x * x + y * y;
                if (sqrDst < radius * radius) {
                    brushOffsets.push_back(x);
                    brushOffsets.push_back(y);
                    float brushWeight = 1 - sqrt(sqrDst) / radius;
                    weightSum += brushWeight;
                    brushWeights.push_back(brushWeight);
                }
            }
        }
        for (int i = 0; i < brushWeights.size(); i++) {
            brushWeights[i] /= weightSum;
        }

        // printf("%d\n", brushWeights.size());

        mErosionShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glUniform1f(mErosionShader->uniformLocation("Time"), rand() / (RAND_MAX + 1.));
        glUniform1f(mErosionShader->uniformLocation("MapHeight"), mMapHeight);
        glUniform1i(mErosionShader->uniformLocation("BorderSize"), borderSize);
        glUniform1i(mErosionShader->uniformLocation("BrushSize"), brushWeights.size());
        glUniform1fv(mErosionShader->uniformLocation("BrushWeights"), brushWeights.size(), brushWeights.data());
        glUniform2iv(mErosionShader->uniformLocation("BrushOffsets"), brushOffsets.size(), brushOffsets.data());

        glBindImageTexture(0, mTexture->handle(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RED);
        glDispatchCompute(count, 1, 1);
    }
}
