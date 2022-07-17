#pragma once

#include "CForge/Core/CoreUtility.hpp"
#include <CForge/Graphics/GraphicsUtility.h>

#include <../../../CForge/Graphics/RenderDevice.h>"

#include <CForge/Graphics/STextureManager.h>

#include "../../../CForge/Graphics/Shader/SShaderManager.h"

#include <glad/glad.h>
#include <iostream>

using namespace CForge;
using namespace std;

class OceanObject {
public:

	OceanObject(int gridSize, float scale, float amplitudeScale, float choppyness, float windX, float windZ, int numInstances, float nearPlane, float farPlane) { //, Eigen::Vector3f offset, Eigen::Vector3f scale) {
		mGridSize = gridSize;
		mScale = scale;
		mAmplitudeScale = amplitudeScale == 0 ? 0.00001 : amplitudeScale;
		mChoppyness = choppyness;
		mWindDirectionX = windX;
		mWindDirectionZ = windZ;
		mNumInstances = numInstances;
		mNearPlane = nearPlane;
		mFarPlane = farPlane;
	}

	void init() {
		initShader();
		initTextures();
		initRenderObjects();
		initFBO(5120 / 4, 1440 / 2);
	}

	void render(RenderDevice* renderDevice, float timeCount) {
		renderDevice->activeShader(mShader);
		glBindVertexArray(mVAO);

		// init uniforms
		int amplitudeScale = mShader->uniformLocation("amplitudeScale");
		int choppyness = mShader->uniformLocation("choppyness");
		int widthScale = mShader->uniformLocation("widthScale");
		int time = mShader->uniformLocation("time");
		int windDirection = mShader->uniformLocation("windDirection");
		int nearFar = mShader->uniformLocation("nearFarPlane");

		// set uniforms
		glUniform1f(amplitudeScale, (float)mAmplitudeScale);
		glUniform1f(choppyness, (float)mChoppyness);
		glUniform1f(widthScale, (float)mScale / (float)mGridSize);
		glUniform1f(time, timeCount);
		glUniform2f(windDirection, mWindDirectionX, mWindDirectionZ);
		glUniform2f(nearFar, mNearPlane, mFarPlane);

		//glDrawElements(GL_TRIANGLES, (mGridSize - 1) * (mGridSize - 1) * 3 * 2, GL_UNSIGNED_INT, 0);

		int foamT = mShader->uniformLocation("foamTexture");
		int colorT = mShader->uniformLocation("colorTexture");
		int depthT = mShader->uniformLocation("depthTexture");
		int posT = mShader->uniformLocation("posTexture");

		glUniform1i(foamT, 0);
		glUniform1i(colorT, 1);
		glUniform1i(depthT, 2);
		glUniform1i(posT, 3);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mFoamTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mColorTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mDepthTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mPositionTexture);
		
	

		glDrawElementsInstanced(GL_TRIANGLES, (mGridSize) * (mGridSize) * 3 * 2, GL_UNSIGNED_INT, 0, mAmount);
	}

	void initTextures() {

		//load foam texture
		T2DImage<uint8_t> image;

		try {
			SAssetIO::load("Assets/richard/oceanfoam.jpg", &image); // https://antoniospg.github.io/UnityOcean/OceanSimulation.html
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
		}

		glGenTextures(1, &mFoamTexture);
		glBindTexture(GL_TEXTURE_2D, mFoamTexture);
		glTextureParameteri(mFoamTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(mFoamTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
	}

	void initFBO(int width, int height) {
		glGenFramebuffers(1, &mfbo);
		glBindFramebuffer(GL_FRAMEBUFFER, mfbo);

		glGenTextures(1, &mColorTexture);
		glBindTexture(GL_TEXTURE_2D, mColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);

		glGenTextures(1, &mPositionTexture);
		glBindTexture(GL_TEXTURE_2D, mPositionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mPositionTexture, 0);

		glGenTextures(1, &mDepthTexture);
		glBindTexture(GL_TEXTURE_2D, mDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthTexture, 0);
	}


	void copyFBO(unsigned int fbo, int width, int height) {
		//color
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mfbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, width - 1, height - 1, 0, 0, width - 1, height - 1, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//depth
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glReadBuffer(GL_DEPTH_ATTACHMENT);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mfbo);
		glBlitFramebuffer(0, 0, width - 1, height - 1, 0, 0, width - 1, height - 1, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		
		//position
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mfbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, width - 1, height - 1, 0, 0, width - 1, height - 1, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}



private:
	float mScale;
	float mGridSize, mAmplitudeScale, mChoppyness, mWindDirectionX, mWindDirectionZ;
	int mAmount = 0;
	int mNumInstances;
	float mNearPlane, mFarPlane;
	vector<float> mBufferData;
	vector<unsigned int> mIndicesBuffer;
	GLShader* mShader;
	unsigned int mFoamTexture, mColorTexture, mDepthTexture, mPositionTexture;
	unsigned int mfbo;
	unsigned int mVBO, mVAO, mEBO;

	void initShader() {
		SShaderManager* shaderManager = SShaderManager::instance();

		vector<ShaderCode*> vsSources;
		vector<ShaderCode*> fsSources;
		string errorLog;

		ShaderCode* vertexShader =
			shaderManager->createShaderCode("Shader/Ocean.vert", "430 core",
				0, "", "");
		ShaderCode* fragmentShader =
			shaderManager->createShaderCode("Shader/Ocean.frag", "430 core",
				0, "", "");

		vsSources.push_back(vertexShader);
		fsSources.push_back(fragmentShader);

		mShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);

		shaderManager->release();
	}


	void initRenderObjects() {
		
		for (int x = 0; x < mGridSize + 1; x++) {
			for (int z = 0; z < mGridSize + 1; z++) {
				// vertex Pos
				mBufferData.push_back((-0.5f + (float)x / mGridSize) * mScale);
				mBufferData.push_back(202);
				mBufferData.push_back((-0.5f + (float)z / mGridSize) * mScale);

				// UV

				mBufferData.push_back((float)x / mGridSize);
				mBufferData.push_back((float)z / mGridSize);
			}
		}



		
		for (int x = 1; x < mGridSize + 1; x++) {
			for (int z = 1; z < mGridSize + 1; z++) {
				int i = x * (mGridSize + 1) + z;
				mIndicesBuffer.push_back(i);
				mIndicesBuffer.push_back(i - mGridSize - 1);
				mIndicesBuffer.push_back(i - mGridSize - 2);
				
				mIndicesBuffer.push_back(i);
				mIndicesBuffer.push_back(i - mGridSize - 2);
				mIndicesBuffer.push_back(i - 1);
			}
		}

		for (int x = -mNumInstances / 2; x < mNumInstances - mNumInstances / 2; x++) {
			for (int z = -mNumInstances / 2; z < mNumInstances - mNumInstances / 2; z++) {

				mBufferData.push_back(x * (mScale));// -(float)mScale / mGridSize));
				mBufferData.push_back(0);
				mBufferData.push_back(z * (mScale));// -(float)mScale / mGridSize));

				mAmount += 1;
			}
		}
		


		glGenVertexArrays(1, &mVAO);
	
		glGenBuffers(1, &mVBO);
		glGenBuffers(1, &mEBO);

		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, mBufferData.size() * sizeof(float), mBufferData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndicesBuffer.size() * sizeof(unsigned int), mIndicesBuffer.data(), GL_STATIC_DRAW);

		//vertex Pos
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//uv
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//instance offset
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)((mBufferData.size() - mAmount * 3) * sizeof(float)));
		glVertexAttribDivisor(2, 1);


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};