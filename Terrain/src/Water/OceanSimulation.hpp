#pragma once

#include "CForge/Core/CoreUtility.hpp"
#include <CForge/Graphics/GraphicsUtility.h>
#include <../../../CForge/Graphics/RenderDevice.h>"
#include <CForge/Graphics/STextureManager.h>
#include "../../../CForge/Graphics/Shader/SShaderManager.h"
#include <glad/glad.h>
#include <iostream>
#include "./OceanObject.hpp"

using namespace CForge;
using namespace std;

bool compareReverse(const int& a, const int& b) {
	int compareBit = 1;
	while (compareBit != 0) {
		if ((a & compareBit) != (b & compareBit)) {
			return (a & compareBit) < (b & compareBit);
		}
		compareBit = compareBit << 1;
	}
	return false;
}

class OceanSimulation {
public:
	OceanSimulation(int gridSize, int horizontalDimension, float amplitude, Eigen::Vector2f windDirection, float windVelocity) {
		mGridSize = gridSize;
		mHorizontalDimension = horizontalDimension;
		mAmplitude = amplitude;
		mWindDirection = windDirection;
		mWindVelocity = windVelocity;
	}

	enum eTextures {
		H0,
		H0_INVERSE,
		Hdx,
		Hdy,
		Hdz,
		TWIDDLE_INDICES,
		DISPLACEMENT,
		NORMALS,
		PINGPONG,
		TEXTURES_AMOUNT
	};

	unsigned int mTextures[TEXTURES_AMOUNT];

	void initOceanSimulation() {
		// init compute shaders
		initShaders();
		// init textures
		initTextures();

		// pre calc H0(k) and H0(-k)
		preCalcH0();
		// precalc butterfly texture
		preCalcTwiddleTexture();
	}

	void updateWaterSimulation(float timeCount) {
		// calc H(k, t)
		updateHdTexture(timeCount);

		for (int i = Hdx; i <= Hdz; i++) {
			// reverse fft calculation
			butterflyOperation(i);
			// final height displacement
			calcDisplacement(i);
		}
	}



private:
	GLShader* mPreCompShader;
	GLShader* mRenderLoopShader;
	GLShader* mButterFlyTextureShader;
	GLShader* mButterFlyShader;
	GLShader* mInversionShader;

	GLuint mBitReversedIndices;

	int mGridSize;
	int mHorizontalDimension;
	float mAmplitude;
	Eigen::Vector2f mWindDirection;
	float mWindVelocity;

	int mPingPong = 0;
	

	float randF(float min, float max) {
		return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
	}

	void initShaders() {
		vector<ShaderCode*> csSources;
		string errorLog;
		SShaderManager* shaderManager = SShaderManager::instance();
		csSources.push_back(shaderManager->createShaderCode("Shader/PreCompShader.comp", "430", 0, "", ""));
		mPreCompShader = shaderManager->buildComputeShader(&csSources, &errorLog);
		csSources.clear();
		csSources.push_back(shaderManager->createShaderCode("Shader/RenderLoopShader.comp", "430", 0, "", ""));
		mRenderLoopShader = shaderManager->buildComputeShader(&csSources, &errorLog);
		csSources.clear();
		csSources.push_back(shaderManager->createShaderCode("Shader/ButterFlyTexture.comp", "430", 0, "", ""));
		mButterFlyTextureShader = shaderManager->buildComputeShader(&csSources, &errorLog);
		csSources.clear();
		csSources.push_back(shaderManager->createShaderCode("Shader/ButterFly.comp", "430", 0, "", ""));
		mButterFlyShader = shaderManager->buildComputeShader(&csSources, &errorLog);
		csSources.clear();
		csSources.push_back(shaderManager->createShaderCode("Shader/InversionShader.comp", "430", 0, "", ""));
		mInversionShader = shaderManager->buildComputeShader(&csSources, &errorLog);
		shaderManager->release();
	}

	void initTextures() {
		for (int i = 0; i < TEXTURES_AMOUNT; i++) {
			glGenTextures(1, &mTextures[i]);
			glBindTexture(GL_TEXTURE_2D, mTextures[i]);
			glTextureParameteri(mTextures[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(mTextures[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			if (i == TWIDDLE_INDICES) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ((int)log2(mGridSize)), mGridSize, 0, GL_RGBA, GL_FLOAT, NULL);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mGridSize, mGridSize, 0, GL_RGBA, GL_FLOAT, NULL);
			}
		}
	}

	void preCalcH0() {
		// bind shader + texures
		glActiveTexture(GL_TEXTURE0);
		glActiveTexture(GL_TEXTURE1);

		mPreCompShader->bind();
		glBindImageTexture(0, mTextures[H0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(1, mTextures[H0_INVERSE], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		// init uniforms
		int randomOffset1 = mPreCompShader->uniformLocation("randomOffset1");
		int randomOffset2 = mPreCompShader->uniformLocation("randomOffset2");
		int randomOffset3 = mPreCompShader->uniformLocation("randomOffset3");
		int randomOffset4 = mPreCompShader->uniformLocation("randomOffset4");
		int gridSize = mPreCompShader->uniformLocation("gridSize");
		int horizontalDimension = mPreCompShader->uniformLocation("horizontalDimension");
		int amplitude = mPreCompShader->uniformLocation("amplitude");
		int windDirection = mPreCompShader->uniformLocation("windDirection");
		int windVelocity = mPreCompShader->uniformLocation("windVelocity");

		// set uniforms
		srand(time(NULL));
		glUniform2f(randomOffset1, randF(0, 1000), randF(0, 1000));
		glUniform2f(randomOffset2, randF(0, 1000), randF(0, 1000));
		glUniform2f(randomOffset3, randF(0, 1000), randF(0, 1000));
		glUniform2f(randomOffset4, randF(0, 1000), randF(0, 1000));
		glUniform1i(gridSize, mGridSize);
		glUniform1i(horizontalDimension, mHorizontalDimension);
		glUniform1f(amplitude, mAmplitude);
		glUniform2f(windDirection, mWindDirection.x(), mWindDirection.y());
		glUniform1f(windVelocity, mWindVelocity);

		// run shader
		glDispatchCompute(mGridSize / 8, mGridSize / 8, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);
	}

	void preCalcTwiddleTexture() {
		mButterFlyTextureShader->bind();
		glBindImageTexture(0, mTextures[TWIDDLE_INDICES], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		vector<int> indicesVector;
		for (int i = 0; i < mGridSize; i++) {
			indicesVector.push_back(i);
		}

		sort(indicesVector.begin(), indicesVector.end(), compareReverse);

		/*
		for (int i = 0; i < mGridSize; i++) {
			cout << indicesVector[i] << "\n";
		}
		*/

		glGenBuffers(1, &mBitReversedIndices);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBitReversedIndices);
		glBufferData(GL_SHADER_STORAGE_BUFFER, mGridSize * sizeof(int), indicesVector.data(), GL_STATIC_READ);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mBitReversedIndices);

		int gridSize = mButterFlyTextureShader->uniformLocation("gridSize");
		glUniform1i(gridSize, mGridSize);

		glDispatchCompute(((int)log2(mGridSize)), mGridSize / 32, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);
	}

	void updateHdTexture(float timeCount) {
		glActiveTexture(GL_TEXTURE0);
		glActiveTexture(GL_TEXTURE1);
		glActiveTexture(GL_TEXTURE2);
		glActiveTexture(GL_TEXTURE3);
		glActiveTexture(GL_TEXTURE4);
		glActiveTexture(GL_TEXTURE5);
		glActiveTexture(GL_TEXTURE6);
		glActiveTexture(GL_TEXTURE7);
		glActiveTexture(GL_TEXTURE8);
		glActiveTexture(GL_TEXTURE9);

		// bind shader + texures
		mRenderLoopShader->bind();

		glBindImageTexture(0, mTextures[Hdx], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(1, mTextures[Hdy], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(2, mTextures[Hdz], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(3, mTextures[H0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(4, mTextures[H0_INVERSE], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

		int horizontalDimension = mRenderLoopShader->uniformLocation("horizontalDimension");
		int time = mRenderLoopShader->uniformLocation("time");

		glUniform1i(horizontalDimension, mHorizontalDimension);
		glUniform1f(time, timeCount);

		glDispatchCompute(mGridSize / 8, mGridSize / 8, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);
	}

	void butterflyOperation(int texture) {
		mButterFlyShader->bind();

		glBindImageTexture(0, mTextures[TWIDDLE_INDICES], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, mTextures[texture], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(2, mTextures[PINGPONG], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		int stage = mButterFlyShader->uniformLocation("stage");
		int pingpong = mButterFlyShader->uniformLocation("pingpong");
		int direction = mButterFlyShader->uniformLocation("direction");

		//horizontal 1d fft
		for (int i = 0; i < log2(mGridSize); i++) {
			glUniform1i(stage, i);
			glUniform1i(pingpong, mPingPong);
			glUniform1i(direction, 0);

			glDispatchCompute(mGridSize / 8, mGridSize / 8, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);

			mPingPong++;
			mPingPong %= 2;
		}
		//vertical 1d fft
		for (int i = 0; i < log2(mGridSize); i++) {
			glUniform1i(stage, i);
			glUniform1i(pingpong, mPingPong);
			glUniform1i(direction, 1);

			glDispatchCompute(mGridSize / 8, mGridSize / 8, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);

			mPingPong++;
			mPingPong %= 2;
		}
	}

	void calcDisplacement(int texture) {
		mInversionShader->bind();

		glBindImageTexture(0, mTextures[DISPLACEMENT], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, mTextures[NORMALS], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(2, mTextures[texture], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(3, mTextures[PINGPONG], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

		int gridSize = mInversionShader->uniformLocation("gridSize");
		int pingpong = mInversionShader->uniformLocation("pingpong");
		int text = mInversionShader->uniformLocation("text");

		glUniform1i(gridSize, mGridSize);
		glUniform1i(pingpong, mPingPong);
		glUniform1i(text, texture - Hdx);

		glDispatchCompute(mGridSize / 8, mGridSize / 8, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);
	}
};


