#include <CForge/Graphics/OpenGLHeader.h>

#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/Graphics/STextureManager.h>

#include "HeightMap.h"

using namespace std;

float randf() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}



namespace Terrain {

    float lerp(float x, float y, float alpha) {
        return x + alpha * (y - x);
    }

    HeightMap::HeightMap() : mTexture(), mHeights() {
        initShader();
    }


    void HeightMap::generate(HeightMapConfig config) {
        mConfig = config;

        GLint internalFormat = GL_R32F;
        GLint format = GL_RED;
        GLint dataType = GL_FLOAT;
        GLenum target = GL_TEXTURE_2D;

        // initialize texture
        GLuint textureHandle;
        glGenTextures(1, &textureHandle);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(target, textureHandle);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(target, 0, internalFormat, config.width, config.height, 0, format, dataType, NULL); 

        // generate map from noise
        mHeightMapShader->bind();
        glBindImageTexture(0, textureHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, internalFormat);
        bindNoiseData(config.noiseConfig);

        glDispatchCompute(config.width, config.height, 1);
        // wait for compute shader to finish
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);

        mTexture = STextureManager::fromHandle(textureHandle);
        //erode(300);
        delete mHeights;
        mHeights = new GLfloat[config.width * config.height];
        glGetTexImage(target, 0, format, dataType, mHeights);
    }

    void HeightMap::updateHeights() {
        GLint format = GL_RED;
        GLint dataType = GL_FLOAT;
        GLenum target = GL_TEXTURE_2D;

        bindTexture();

        delete mHeights;
        mHeights = new GLfloat[mConfig.width * mConfig.height];
        glGetTexImage(target, 0, format, dataType, mHeights);
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
                                                            0, ""));
        mHeightMapShader = shaderManager->buildComputeShader(&csSources, &errorLog);

        csSources.clear();
        csSources.push_back(shaderManager->createShaderCode("Shader/Erosion.comp", "430",
                                                            0, ""));
        mErosionShader = shaderManager->buildComputeShader(&csSources, &errorLog);

        shaderManager->release();
    }

    void HeightMap::erode(int32_t count) {
        int radius = 6;
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

        // printf("%zu\n", brushWeights.size());

        mErosionShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glUniform1f(mErosionShader->uniformLocation("Time"), rand() / (RAND_MAX + 1.));
        glUniform1f(mErosionShader->uniformLocation("MapHeight"), mConfig.mapHeight);
        glUniform1i(mErosionShader->uniformLocation("BorderSize"), borderSize);
        glUniform1i(mErosionShader->uniformLocation("BrushSize"), brushWeights.size());
        glUniform1fv(mErosionShader->uniformLocation("BrushWeights"), brushWeights.size(), brushWeights.data());
        glUniform2iv(mErosionShader->uniformLocation("BrushOffsets"), brushOffsets.size(), brushOffsets.data());

        //glBindImageTexture(0, mTexture->handle(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RED);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);
        glDispatchCompute(count, 1, 1);
        // wait for compute shader to finish
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);
    }

    float HeightMap::getHeightAt(float x, float y) {
        int32_t coord_x = x;
        int32_t coord_y = y;

        float offset_x = x - coord_x;
        float offset_y = y - coord_y;

		coord_x = std::clamp(coord_x, (int32_t) 0, (int32_t) mConfig.height-2);
		coord_y = std::clamp(coord_y, (int32_t) 0, (int32_t) mConfig.width-2);

        float s00 = mHeights[coord_x + mConfig.height * coord_y];
        float s10 = mHeights[coord_x + mConfig.height * coord_y + 1];
        float s01 = mHeights[coord_x + mConfig.height * (coord_y + 1)];
        float s11 = mHeights[coord_x + mConfig.height * (coord_y + 1) + 1];


        float height = lerp(lerp(s00, s10, offset_x), lerp(s01, s11, offset_x), offset_y);

        // printf("%f %f %f\n", x, height * mConfig.mapHeight, y);

        return height * mConfig.mapHeight;
    }

    const HeightMap::HeightMapConfig &HeightMap::getConfig() {
        return mConfig;
    }

    Vector3f HeightMap::getNormalAt(float x, float y) {
        float s01 = getHeightAt(x - 1, y);
        float s21 = getHeightAt(x + 1, y);
        float s10 = getHeightAt(x, y - 1);
        float s12 = getHeightAt(x, y + 1);

        Vector3f normal = Vector3f(s21 - s01, 2, s12 - s10);
        normal.normalize();

        // printf("%f %f %f\n", normal.x(), normal.y(), normal.z());

        return normal;
    }

    void HeightMap::setConfig(HeightMapConfig config) {
        mConfig = config;
    }
}
