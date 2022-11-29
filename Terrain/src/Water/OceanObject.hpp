#pragma once

#include "CForge/Core/CoreUtility.hpp"
#include "CForge/Graphics/GraphicsUtility.h"

#include "../../../CForge/Graphics/RenderDevice.h"

#include "CForge/Graphics/STextureManager.h"

#include "../../../CForge/Graphics/Shader/SShaderManager.h"

#include "Terrain/src/Map/TerrainMap.h"

#include "spline.h"
#include "River.hpp"
#include "Settings.hpp"


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

	void init(float screenWidth, float screenHeight) {
		initShader();
		initTextures();
		initRenderObjects();
		initFBO(screenWidth, screenHeight);
	}

	void renderPool(RenderDevice* renderDevice, float timeCount) {
		renderDevice->activeShader(mPoolShader);
		glBindVertexArray(mPoolVAO);

		int time = mPoolShader->uniformLocation("time");
		int windDirection = mPoolShader->uniformLocation("windDirection");
		int nearFar = mPoolShader->uniformLocation("nearFarPlane");
		int uSSR = mPoolShader->uniformLocation("doSSR");
		int uSSRRes = mPoolShader->uniformLocation("ssrRes");

		glUniform1f(time, timeCount);
		glUniform2f(windDirection, mWindDirectionX, mWindDirectionZ);
		glUniform2f(nearFar, mNearPlane, mFarPlane);
		glUniform1i(uSSR, gSettings.ssr);
		glUniform1f(uSSRRes, gSettings.ssrResolution);

		int dudvT = mPoolShader->uniformLocation("dudvTexture");
		int colorT = mPoolShader->uniformLocation("colorTexture");
		int depthT = mPoolShader->uniformLocation("depthTexture");
		int posT = mPoolShader->uniformLocation("posTexture");
		int normalT = mPoolShader->uniformLocation("normalTexture");
		int worldPosT = mPoolShader->uniformLocation("worldPosTexture");
		int wCausticsT = mPoolShader->uniformLocation("waterCausticsTexture");

		int backT = mPoolShader->uniformLocation("skyBackTexture");
		int botT = mPoolShader->uniformLocation("skyBotTexture");
		int frontT = mPoolShader->uniformLocation("skyFrontTexture");
		int leftT = mPoolShader->uniformLocation("skyLeftTexture");
		int rightT = mPoolShader->uniformLocation("skyRightTexture");
		int topT = mPoolShader->uniformLocation("skyTopTexture");

		glUniform1i(dudvT, 0);
		glUniform1i(colorT, 1);
		glUniform1i(depthT, 2);
		glUniform1i(posT, 3);
		glUniform1i(normalT, 4);
		glUniform1i(worldPosT, 5);
		glUniform1i(wCausticsT, 6);

		glUniform1i(backT, 7);
		glUniform1i(botT, 8);
		glUniform1i(frontT, 9);
		glUniform1i(leftT, 10);
		glUniform1i(rightT, 11);
		glUniform1i(topT, 12);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mPoolDUDVTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mColorTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mDepthTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mPositionTexture);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mPoolNormalTexture);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, mPositionTexture);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, mWaterCausticsTexture);

		for (int i = 0; i < 6; i++) {
			glActiveTexture(GL_TEXTURE7 + i);
			glBindTexture(GL_TEXTURE_2D, mSkyboxTextures[i]);
		}

		glDrawElements(GL_TRIANGLES, mPoolBufferData.size() * 3 * 2, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}

	void renderStreams(RenderDevice* renderDevice, float timeCount) {
		renderDevice->activeShader(mStreamShader);
		glBindVertexArray(mStreamVAO);

		int time = mStreamShader->uniformLocation("time");
		int windDirection = mStreamShader->uniformLocation("windDirection");
		int nearFar = mStreamShader->uniformLocation("nearFarPlane");
		int uSSR = mStreamShader->uniformLocation("doSSR");
		int uSSRRes = mStreamShader->uniformLocation("ssrRes");

		glUniform1f(time, timeCount);
		glUniform2f(windDirection, mWindDirectionX, mWindDirectionZ);
		glUniform2f(nearFar, mNearPlane, mFarPlane);
		glUniform1i(uSSR, gSettings.ssr);
		glUniform1f(uSSRRes, gSettings.ssrResolution);

		int dudvT = mStreamShader->uniformLocation("dudvTexture");
		int colorT = mStreamShader->uniformLocation("colorTexture");
		int depthT = mStreamShader->uniformLocation("depthTexture");
		int posT = mStreamShader->uniformLocation("posTexture");
		int normalT = mStreamShader->uniformLocation("normalTexture");
		int foamT = mStreamShader->uniformLocation("foamTexture");
		int foamBlendT = mStreamShader->uniformLocation("foamBlendTexture");
		int worldPosT = mStreamShader->uniformLocation("worldPosTexture");

		int backT = mStreamShader->uniformLocation("skyBackTexture");
		int botT = mStreamShader->uniformLocation("skyBotTexture");
		int frontT = mStreamShader->uniformLocation("skyFrontTexture");
		int leftT = mStreamShader->uniformLocation("skyLeftTexture");
		int rightT = mStreamShader->uniformLocation("skyRightTexture");
		int topT = mStreamShader->uniformLocation("skyTopTexture");

		glUniform1i(dudvT, 0);
		glUniform1i(colorT, 1);
		glUniform1i(depthT, 2);
		glUniform1i(posT, 3);
		glUniform1i(normalT, 4);
		glUniform1i(foamT, 5);
		glUniform1i(foamBlendT, 6);
		glUniform1i(worldPosT, 7);

		glUniform1i(backT, 8);
		glUniform1i(botT, 9);
		glUniform1i(frontT, 10);
		glUniform1i(leftT, 11);
		glUniform1i(rightT, 12);
		glUniform1i(topT, 13);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mPoolDUDVTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mColorTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mDepthTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mPositionTexture);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mPoolNormalTexture);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, mFoamTexture);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, mFoamBlendTexture);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, mPositionTexture);

		for (int i = 0; i < 6; i++) {
			glActiveTexture(GL_TEXTURE8 + i);
			glBindTexture(GL_TEXTURE_2D, mSkyboxTextures[i]);
		}

		glDrawElements(GL_TRIANGLES, mStreamIndicesBuffer.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}

	void renderOcean(RenderDevice* renderDevice, float timeCount, unsigned int heightMapTextureHandle, unsigned int shoreDistTextureHandle, unsigned int oceanDisplacementTextureHandle, unsigned int oceanNormalsTextureHandle) {
		renderDevice->activeShader(mShader);
		glBindVertexArray(mVAO);

		// init uniforms
		int amplitudeScale = mShader->uniformLocation("amplitudeScale");
		int choppyness = mShader->uniformLocation("choppyness");
		int widthScale = mShader->uniformLocation("widthScale");
		int time = mShader->uniformLocation("time");
		int windDirection = mShader->uniformLocation("windDirection");
		int nearFar = mShader->uniformLocation("nearFarPlane");
		int uvScale = mShader->uniformLocation("uvScale");
		int uSSR = mShader->uniformLocation("doSSR");
		int uSSRRes = mShader->uniformLocation("ssrRes");
		int uShoreWave = mShader->uniformLocation("doShoreWave");
		int uSizeScale = mShader->uniformLocation("sSizeScale");

		// set uniforms
		glUniform1f(amplitudeScale, (float)mAmplitudeScale);
		glUniform1f(choppyness, (float)mChoppyness);
		glUniform1f(widthScale, (float)mScale / (float)mGridSize);
		glUniform1f(time, timeCount);
		glUniform2f(windDirection, mWindDirectionX, mWindDirectionZ);
		glUniform2f(nearFar, mNearPlane, mFarPlane);
		glUniform1f(uvScale, mScale);
		glUniform1i(uSSR, gSettings.ssr);
		glUniform1f(uSSRRes, gSettings.ssrResolution);
		glUniform1i(uShoreWave, gSettings.shoreWaves);
		glUniform1f(uSizeScale, gSettings.sizeScale);

		//glDrawElements(GL_TRIANGLES, (mGridSize - 1) * (mGridSize - 1) * 3 * 2, GL_UNSIGNED_INT, 0);

		int foamT = mShader->uniformLocation("foamTexture");
		int colorT = mShader->uniformLocation("colorTexture");
		int depthT = mShader->uniformLocation("depthTexture");
		int foamBlendT = mShader->uniformLocation("foamBlendTexture");
		int heightMapT = mShader->uniformLocation("heightMapTexture");
		int shoreDistT = mShader->uniformLocation("shoreDistTexture");
		int worldPosT = mShader->uniformLocation("worldPosTexture");
		int wCausticsT = mShader->uniformLocation("waterCausticsTexture");
		int displacementT = mShader->uniformLocation("displacementTexture");
		int normalsT = mShader->uniformLocation("normalsTexture");

		int backT = mShader->uniformLocation("skyBackTexture");
		int botT = mShader->uniformLocation("skyBotTexture");
		int frontT = mShader->uniformLocation("skyFrontTexture");
		int leftT = mShader->uniformLocation("skyLeftTexture");
		int rightT = mShader->uniformLocation("skyRightTexture");
		int topT = mShader->uniformLocation("skyTopTexture");

		glUniform1i(foamT, 0);
		glUniform1i(colorT, 1);
		glUniform1i(depthT, 2);
		glUniform1i(foamBlendT, 3);
		glUniform1i(heightMapT, 4);
		glUniform1i(shoreDistT, 5);
		glUniform1i(worldPosT, 6);
		glUniform1i(wCausticsT, 7);
		glUniform1i(displacementT, 8);
		glUniform1i(normalsT, 9);

		glUniform1i(backT, 10);
		glUniform1i(botT, 11);
		glUniform1i(frontT, 12);
		glUniform1i(leftT, 13);
		glUniform1i(rightT, 14);
		glUniform1i(topT, 15);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mFoamTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mColorTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mDepthTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mFoamBlendTexture);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, heightMapTextureHandle);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, shoreDistTextureHandle);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, mPositionTexture);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, mWaterCausticsTexture);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, oceanDisplacementTextureHandle);
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, oceanNormalsTextureHandle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
		
		for (int i = 0; i < 6; i++) {
			glActiveTexture(GL_TEXTURE10 + i);
			glBindTexture(GL_TEXTURE_2D, mSkyboxTextures[i]);
		}

		glDrawElementsInstanced(GL_TRIANGLES, (mGridSize) * (mGridSize) * 3 * 2, GL_UNSIGNED_INT, 0, mAmount);

		glBindVertexArray(mOceanBorderVAO);
		glDrawElementsInstanced(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0, 1);
	}

	void blurHReflection(RenderDevice* renderDevice) {
		glBindFramebuffer(GL_FRAMEBUFFER, mfbo3);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		

		renderDevice->activeShader(mHBlurShader);
		glBindVertexArray(mScreenQuadVAO);

		int blurS = mHBlurShader->uniformLocation("blurStrength");
		glUniform1i(blurS, 1);


		int bColorT = mHBlurShader->uniformLocation("baseColorTexture");
		int rColorT = mHBlurShader->uniformLocation("reflectColorTexture");

		glUniform1i(bColorT, 0);
		glUniform1i(rColorT, 1);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mWaterRenderTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mWaterRenderReflectTexture);

		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void blurVReflection(RenderDevice* renderDevice) {
		glBindFramebuffer(GL_FRAMEBUFFER, mfbo2);
		renderDevice->activeShader(mVBlurShader);
		glBindVertexArray(mScreenQuadVAO);

		int blurS = mVBlurShader->uniformLocation("blurStrength");
		glUniform1i(blurS, 1);

		int bColorT = mVBlurShader->uniformLocation("baseColorTexture");
		int helpT = mVBlurShader->uniformLocation("helpTexture");

		glUniform1i(bColorT, 0);
		glUniform1i(helpT, 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mWaterRenderTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mBlurTexture);

		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void renderWater(RenderDevice* renderDevice, float timeCount, unsigned int heightMapTextureHandle, unsigned int shoreDistTextureHandle, unsigned int oceanDisplacementTextureHandle, unsigned int oceanNormalsTextureHandle) {
		renderOcean(renderDevice, timeCount, heightMapTextureHandle, shoreDistTextureHandle, oceanDisplacementTextureHandle, oceanNormalsTextureHandle);
		renderPool(renderDevice, timeCount);
		renderStreams(renderDevice, timeCount);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (gSettings.blur) {
			blurHReflection(renderDevice);
			blurVReflection(renderDevice);
		}
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		renderDevice->activeShader(mPostProcessShader);
		glBindVertexArray(mScreenQuadVAO);

		int bColorT = mPostProcessShader->uniformLocation("baseColorTexture");
		int rColorT = mPostProcessShader->uniformLocation("reflectColorTexture");

		glUniform1i(bColorT, 0);
		glUniform1i(rColorT, 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mWaterRenderTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mWaterRenderReflectTexture);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}


	void initTextures() {

		//load foam texture
		T2DImage<uint8_t> image;

		try {
			SAssetIO::load("Assets/richard/ocean_foam.png", &image); // https://youtu.be/oI_q5g3580I
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
		}

		glGenTextures(1, &mFoamTexture);
		glBindTexture(GL_TEXTURE_2D, mFoamTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		glGenerateMipmap(GL_TEXTURE_2D);

		//foam blend textures
		try {
			SAssetIO::load("Assets/richard/ocean_foam2_blend_ramp.png", &image); // https://www.youtube.com/watch?v=qgDPSnZPGMA&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh&index=8
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
		}

		glGenTextures(1, &mFoamBlendTexture);
		glBindTexture(GL_TEXTURE_2D, mFoamBlendTexture);
		glTextureParameteri(mFoamBlendTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mFoamBlendTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());

		//load pool textures
		try {
			SAssetIO::load("Assets/richard/poolDUDV.png", &image); // https://www.youtube.com/watch?v=qgDPSnZPGMA&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh&index=8
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
		}

		glGenTextures(1, &mPoolDUDVTexture);
		glBindTexture(GL_TEXTURE_2D, mPoolDUDVTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		glGenerateMipmap(GL_TEXTURE_2D);

		try {
			SAssetIO::load("Assets/richard/poolNormal.png", &image); // https://www.youtube.com/watch?v=qgDPSnZPGMA&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh&index=8
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
		}

		glGenTextures(1, &mPoolNormalTexture);
		glBindTexture(GL_TEXTURE_2D, mPoolNormalTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		glGenerateMipmap(GL_TEXTURE_2D);

		//skybox textures
		for (int i = 0; i < 6; i++) {
			try {
				SAssetIO::load(mSkyboxTexturePaths[i], &image);
			}
			catch (CrossForgeException& e) {
				SLogger::logException(e);
			}

			glGenTextures(1, &mSkyboxTextures[i]);
			glBindTexture(GL_TEXTURE_2D, mSkyboxTextures[i]);
			glTextureParameteri(mSkyboxTextures[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(mSkyboxTextures[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		}

		//water caustics texture
		try {
			SAssetIO::load("Assets/richard/water_caustics.jpg", &image); // https://www.dualheights.se/caustics/#download
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
		}

		glGenTextures(1, &mWaterCausticsTexture);
		glBindTexture(GL_TEXTURE_2D, mWaterCausticsTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		glGenerateMipmap(GL_TEXTURE_2D);


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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mPositionTexture, 0);

		glGenTextures(1, &mDepthTexture);
		glBindTexture(GL_TEXTURE_2D, mDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthTexture, 0);



		glGenFramebuffers(1, &mfbo2);
		glBindFramebuffer(GL_FRAMEBUFFER, mfbo2);

		// basecolor
		glGenTextures(1, &mWaterRenderTexture);
		glBindTexture(GL_TEXTURE_2D, mWaterRenderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mWaterRenderTexture, 0);

		// reflectcolor
		glGenTextures(1, &mWaterRenderReflectTexture);
		glBindTexture(GL_TEXTURE_2D, mWaterRenderReflectTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mWaterRenderReflectTexture, 0);

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		glGenRenderbuffers(1, &mrbo2);
		glBindRenderbuffer(GL_RENDERBUFFER, mrbo2);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mrbo2);



		glGenFramebuffers(1, &mfbo3);
		glBindFramebuffer(GL_FRAMEBUFFER, mfbo3);

		// blurcolor
		glGenTextures(1, &mBlurTexture);
		glBindTexture(GL_TEXTURE_2D, mBlurTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBlurTexture, 0);

	}

	void bindFBO2(int width, int height) {
		glBindFramebuffer(GL_FRAMEBUFFER, mfbo2);
		glEnable(GL_DEPTH_TEST);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, mfbo);
		glReadBuffer(GL_DEPTH_ATTACHMENT);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mfbo2);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	}

	unsigned int getWaterRenderTextureHandle() {
		return mWaterRenderTexture;
	}

	unsigned int getWaterReflectionTextureHandle() {
		return mWaterRenderReflectTexture;
	}
 
	void copyFBO(unsigned int fbo, int width, int height) {
		//color
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mfbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//depth
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glReadBuffer(GL_DEPTH_ATTACHMENT);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mfbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		
		//position
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mfbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void generateWaterGeometry(Vector2i dimension, float height, float scale, float* heightMap, float* poolMap, vector<River>* riverPoints) {
		mPoolBufferData.clear();
		mPoolIndicesBuffer.clear();

		mStreamBufferData.clear();
		mStreamIndicesBuffer.clear();

		float heightOffset = -0.0f;

		mHeightMapDimension = dimension;

		int poolI = -1;
		int streamI = -1;
		for (int x = 0; x < dimension.x(); x++) {
			for (int y = 0; y < dimension.y(); y++) {
				int index = x * dimension.y() + y;
				
				if (poolMap[index] > 0) {
					mPoolBufferData.push_back((y - dimension.y() / 2.0f - 1) * scale);
					mPoolBufferData.push_back((heightMap[index] + poolMap[index]) * height + heightOffset);
					mPoolBufferData.push_back((x - dimension.x() / 2.0f - 1) * scale);

					mPoolBufferData.push_back((y - dimension.y() / 2.0f + 1) * scale);
					mPoolBufferData.push_back((heightMap[index] + poolMap[index]) * height + heightOffset);
					mPoolBufferData.push_back((x - dimension.x() / 2.0f - 1) * scale);

					mPoolBufferData.push_back((y - dimension.y() / 2.0f - 1) * scale);
					mPoolBufferData.push_back((heightMap[index] + poolMap[index]) * height + heightOffset);
					mPoolBufferData.push_back((x - dimension.x() / 2.0f + 1) * scale);

					mPoolBufferData.push_back((y - dimension.y() / 2.0f + 1) * scale);
					mPoolBufferData.push_back((heightMap[index] + poolMap[index]) * height + heightOffset);
					mPoolBufferData.push_back((x - dimension.x() / 2.0f + 1) * scale);

					poolI += 4;

					mPoolIndicesBuffer.push_back(poolI);
					mPoolIndicesBuffer.push_back(poolI - 2);
					mPoolIndicesBuffer.push_back(poolI - 3);
					 
					mPoolIndicesBuffer.push_back(poolI - 3);
					mPoolIndicesBuffer.push_back(poolI - 1);
					mPoolIndicesBuffer.push_back(poolI);
				}
				/*
				if (streamMap[index] > 0) {
					mStreamBufferData.push_back((y - dimension.y() / 2.0f - 0.5) * scale);
					mStreamBufferData.push_back((heightMap[index - dimension.y() - 1] + 0.0001f) * height + heightOffset);
					mStreamBufferData.push_back((x - dimension.x() / 2.0f - 0.5) * scale);
					 
					mStreamBufferData.push_back((y - dimension.y() / 2.0f + 0.5) * scale);
					mStreamBufferData.push_back((heightMap[index - dimension.y() + 1] + 0.0001f) * height + heightOffset);
					mStreamBufferData.push_back((x - dimension.x() / 2.0f - 0.5) * scale);
					 
					mStreamBufferData.push_back((y - dimension.y() / 2.0f - 0.5) * scale);
					mStreamBufferData.push_back((heightMap[index + dimension.y() - 1] + 0.0001f) * height + heightOffset);
					mStreamBufferData.push_back((x - dimension.x() / 2.0f + 0.5) * scale);
					 
					mStreamBufferData.push_back((y - dimension.y() / 2.0f + 0.5) * scale);
					mStreamBufferData.push_back((heightMap[index + dimension.y() + 1] + 0.0001f) * height + heightOffset);
					mStreamBufferData.push_back((x - dimension.x() / 2.0f + 0.5) * scale);

					streamI += 4;

					mStreamIndicesBuffer.push_back(streamI);
					mStreamIndicesBuffer.push_back(streamI - 2);
					mStreamIndicesBuffer.push_back(streamI - 3);
					
					mStreamIndicesBuffer.push_back(streamI - 3);
					mStreamIndicesBuffer.push_back(streamI - 1);
					mStreamIndicesBuffer.push_back(streamI);
				}
				*/
			}
		}

		genTestRiver(riverPoints, height, scale, dimension);


		
		//pool
		glBindVertexArray(mPoolVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mPoolVBO);
		glBufferData(GL_ARRAY_BUFFER, mPoolBufferData.size() * sizeof(float), mPoolBufferData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPoolEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mPoolIndicesBuffer.size() * sizeof(unsigned int), mPoolIndicesBuffer.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		

		//stream
		glBindVertexArray(mStreamVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mStreamVBO);
		glBufferData(GL_ARRAY_BUFFER, mStreamBufferData.size() * sizeof(float), mStreamBufferData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mStreamEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mStreamIndicesBuffer.size() * sizeof(unsigned int), mStreamIndicesBuffer.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(11 * sizeof(float)));
		glEnableVertexAttribArray(4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void genTestRiver(vector<River>* rivers, float height, float scale, Vector2i dimension) {
		mStreamBufferData.clear();
		mStreamIndicesBuffer.clear();

		float heightOffset = -0.55f;
		
		for (auto river : *rivers) {
			int numSegments = river.getLength() * river.getResolution() / 2;

			float width = 1.5;
			int index = mStreamBufferData.size() / 12 - 1;

			for (int i = 0; i < numSegments; i++) {
				double sampleTime = (double)(i * river.getLength()) / (double)numSegments;
				Vector3f pos = river.getPos(sampleTime);
				float width = river.getWidth(sampleTime);
				//width = width > 6 ? width : 6;
				Vector3f normal = river.getNormal(sampleTime) * (width / 1.8);
				Vector3f tangent = river.get3DTangent(sampleTime);
				Vector3f normal3D = river.get3DNormal(sampleTime);

				pos = pos - normal;

				//pos
				mStreamBufferData.push_back(pos.z());
				mStreamBufferData.push_back(pos.y() + heightOffset);
				mStreamBufferData.push_back(pos.x());
				//uv
				mStreamBufferData.push_back((float)sampleTime);
				mStreamBufferData.push_back(0);
				//normal
				mStreamBufferData.push_back(normal3D.z());
				mStreamBufferData.push_back(normal3D.y());
				mStreamBufferData.push_back(normal3D.x());
				//tangent
				mStreamBufferData.push_back(tangent.z());
				mStreamBufferData.push_back(tangent.y());
				mStreamBufferData.push_back(tangent.x());
				//width
				mStreamBufferData.push_back(width / 2.0f);

				pos = pos + normal * 2;

				mStreamBufferData.push_back(pos.z());
				mStreamBufferData.push_back(pos.y() + heightOffset);
				mStreamBufferData.push_back(pos.x());

				mStreamBufferData.push_back((float)sampleTime);
				mStreamBufferData.push_back(1);

				mStreamBufferData.push_back(normal3D.z());
				mStreamBufferData.push_back(normal3D.y());
				mStreamBufferData.push_back(normal3D.x());
				//tangent
				mStreamBufferData.push_back(tangent.z());
				mStreamBufferData.push_back(tangent.y());
				mStreamBufferData.push_back(tangent.x());
				//width
				mStreamBufferData.push_back(width / 2.0f);

				index += 2;

				if (i > 0) {

					mStreamIndicesBuffer.push_back(index - 3);
					mStreamIndicesBuffer.push_back(index - 2);
					mStreamIndicesBuffer.push_back(index);

					mStreamIndicesBuffer.push_back(index);
					mStreamIndicesBuffer.push_back(index - 1);
					mStreamIndicesBuffer.push_back(index - 3);
				}
			}
		}
	}



private:
	float mScale;
	float mGridSize, mAmplitudeScale, mChoppyness, mWindDirectionX, mWindDirectionZ;
	int mAmount = 0;
	int mNumInstances;
	float mNearPlane, mFarPlane;
	float mSealevel = 25 * gSettings.sizeScale;

	Vector2i mHeightMapDimension;

	vector<float> mBufferData;
	vector<unsigned int> mIndicesBuffer;
	vector<float> mPoolBufferData;
	vector<unsigned int> mPoolIndicesBuffer;
	vector<float> mStreamBufferData;
	vector<unsigned int> mStreamIndicesBuffer;

	GLShader* mShader;
	GLShader* mPoolShader;
	GLShader* mStreamShader;
	GLShader* mPostProcessShader;
	GLShader* mHBlurShader;
	GLShader* mVBlurShader;

	unsigned int mFoamTexture, mColorTexture, mDepthTexture, mPositionTexture, mPoolDUDVTexture, mPoolNormalTexture, mWaterCausticsTexture, mFoamBlendTexture, mWaterRenderTexture, mWaterRenderReflectTexture, mBlurTexture;
	unsigned int mSkyboxTextures[6];
	vector<string> mSkyboxTexturePaths = {"Assets/skybox/back.jpg", "Assets/skybox/bottom.jpg", "Assets/skybox/front.jpg", "Assets/skybox/left.jpg", "Assets/skybox/right.jpg", "Assets/skybox/top.jpg"};
	unsigned int mfbo, mfbo2, mrbo2, mfbo3;

	unsigned int mVBO, mVAO, mEBO;
	unsigned int mOceanBorderVBO, mOceanBorderVAO, mOceanBorderEBO;
	unsigned int mPoolVBO, mPoolVAO, mPoolEBO;
	unsigned int mStreamVBO, mStreamVAO, mStreamEBO;
	unsigned int mScreenQuadVBO, mScreenQuadVAO, mScreenQuadEBO;

	void initShader() {
		SShaderManager* shaderManager = SShaderManager::instance();

		//ocean
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

		//lake
		vsSources.clear();
		fsSources.clear();

		vertexShader =
			shaderManager->createShaderCode("Shader/Pool.vert", "430 core",
				0, "", "");
		fragmentShader =
			shaderManager->createShaderCode("Shader/Pool.frag", "430 core",
				0, "", "");

		vsSources.push_back(vertexShader);
		fsSources.push_back(fragmentShader);

		mPoolShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);

		//postprocess
		vsSources.clear();
		fsSources.clear();

		vertexShader =
			shaderManager->createShaderCode("Shader/WaterPostProcessing.vert", "430 core",
				0, "", "");
		fragmentShader =
			shaderManager->createShaderCode("Shader/WaterPostProcessing.frag", "430 core",
				0, "", "");

		vsSources.push_back(vertexShader);
		fsSources.push_back(fragmentShader);

		mPostProcessShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);

		//blur
		vsSources.clear();
		fsSources.clear();

		vertexShader =
			shaderManager->createShaderCode("Shader/WaterPostProcessing.vert", "430 core",
				0, "", "");
		fragmentShader =
			shaderManager->createShaderCode("Shader/ReflectionBlurHorizontal.frag", "430 core",
				0, "", "");

		vsSources.push_back(vertexShader);
		fsSources.push_back(fragmentShader);

		mHBlurShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);

		vsSources.clear();
		fsSources.clear();

		vertexShader =
			shaderManager->createShaderCode("Shader/WaterPostProcessing.vert", "430 core",
				0, "", "");
		fragmentShader =
			shaderManager->createShaderCode("Shader/ReflectionBlurVertical.frag", "430 core",
				0, "", "");

		vsSources.push_back(vertexShader);
		fsSources.push_back(fragmentShader);

		mVBlurShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
		
		//river
		vsSources.clear();
		fsSources.clear();

		vertexShader =
			shaderManager->createShaderCode("Shader/River.vert", "430 core",
				0, "", "");
		fragmentShader =
			shaderManager->createShaderCode("Shader/River.frag", "430 core",
				0, "", "");

		vsSources.push_back(vertexShader);
		fsSources.push_back(fragmentShader);

		mStreamShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
		
		shaderManager->release();
	}



	void initRenderObjects() {
		
		for (int x = 0; x < mGridSize + 1; x++) {
			for (int z = 0; z < mGridSize + 1; z++) {
				// vertex Pos
				mBufferData.push_back((-0.5f + (float)x / mGridSize) * mScale);
				mBufferData.push_back(mSealevel);
				mBufferData.push_back((-0.5f + (float)z / mGridSize) * mScale);

				// UV

				mBufferData.push_back((float)x / mGridSize);
				mBufferData.push_back((float)z / mGridSize);
			}
		}
		
		for (int x = 1; x < mGridSize + 1; x++) {
			for (int z = 1; z < mGridSize + 1; z++) {
				int i = x * (mGridSize + 1) + z;
				mIndicesBuffer.push_back(i - mGridSize - 2);
				mIndicesBuffer.push_back(i - mGridSize - 1);
				mIndicesBuffer.push_back(i);
				
				mIndicesBuffer.push_back(i - 1);
				mIndicesBuffer.push_back(i - mGridSize - 2);
				mIndicesBuffer.push_back(i);
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
		

		/*
						(2)------------------------(3)
						 |	      					|
						 |	      					|
						 |	      					|
						(8)       (6)------(7)	   (9)
						 |         |########|		|
						 |         |########|		|
						 |         |########|		|
					   (10)       (4)------(5)	   (11)
						 |	      	   				|
						 |	      					|
						 |                          |
						(0)------------------------(1)
		*/

		vector<unsigned int> oceanBorderIndices = {	0, 10, 11,
													0, 11, 1,
													5, 7, 9,
													5, 9, 11,
													10, 8, 6,
													10, 6, 4,
													8, 2, 3,
													8, 3, 9
		};

		float helpOffset = mScale * mNumInstances / 2.0f;
		float outerOffset = mFarPlane;
		float uvOffsetP = outerOffset / mScale + mNumInstances / 2.0f;
		float uvOffsetN = -outerOffset / mScale + mNumInstances / 2.0f;
		vector<float> oceanBorderVertices = {	-outerOffset,	mSealevel,		-outerOffset,	uvOffsetN,				uvOffsetN,
												outerOffset,	mSealevel,		-outerOffset,	uvOffsetP,				uvOffsetN,
												-outerOffset,	mSealevel,		outerOffset,	uvOffsetN,				uvOffsetP,
												outerOffset,	mSealevel,		outerOffset,	uvOffsetP,				uvOffsetP,
												-helpOffset,	mSealevel,		-helpOffset,	0,						0,
												helpOffset,		mSealevel,		-helpOffset,	(float)mNumInstances,	0,
												-helpOffset,	mSealevel,		helpOffset,		0,						(float)mNumInstances,
												helpOffset,		mSealevel,		helpOffset,		(float)mNumInstances,	(float)mNumInstances,
												-outerOffset,	mSealevel,		helpOffset,		uvOffsetN,				(float)mNumInstances,
												outerOffset,	mSealevel,		helpOffset,		uvOffsetP,				(float)mNumInstances,
												-outerOffset,	mSealevel,		-helpOffset,	uvOffsetN,				0,
												outerOffset,	mSealevel,		-helpOffset,	uvOffsetP,				0,
												0, 0, 0
		};


		//screenQuad
		float screenQuadVertices[] = {
			 1.0f,  1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f 
		};
		unsigned int screenQuadIndices[] = {
			3, 1, 0,
			3, 2, 1
		};
		glGenVertexArrays(1, &mScreenQuadVAO);
		glGenBuffers(1, &mScreenQuadVBO);
		glGenBuffers(1, &mScreenQuadEBO);

		glBindVertexArray(mScreenQuadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mScreenQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mScreenQuadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenQuadIndices), screenQuadIndices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);



		// pool buffers
		glGenVertexArrays(1, &mPoolVAO);
		glGenBuffers(1, &mPoolVBO);
		glGenBuffers(1, &mPoolEBO);

		// stream buffers
		glGenVertexArrays(1, &mStreamVAO);
		glGenBuffers(1, &mStreamVBO);
		glGenBuffers(1, &mStreamEBO);

		// ocean bufrfers
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
		

		//OCEAN BORDER
		glGenVertexArrays(1, &mOceanBorderVAO);
		glGenBuffers(1, &mOceanBorderVBO);
		glGenBuffers(1, &mOceanBorderEBO);

		glBindVertexArray(mOceanBorderVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mOceanBorderVBO);
		glBufferData(GL_ARRAY_BUFFER, oceanBorderVertices.size() * sizeof(float), oceanBorderVertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mOceanBorderEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, oceanBorderIndices.size() * sizeof(unsigned int), oceanBorderIndices.data(), GL_STATIC_DRAW);

		//vertex Pos
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//uv
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//instance offset
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)((oceanBorderVertices.size() - 3) * sizeof(float)));
		glVertexAttribDivisor(2, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}
};