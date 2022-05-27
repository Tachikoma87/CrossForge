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
	OceanObject(int gridSize) { //, Eigen::Vector3f offset, Eigen::Vector3f scale) {
		mGridSize = gridSize;
	}

	void init() {
		initShader();
		initRenderObjects();

	}

	void render(RenderDevice* renderDevice) {
		renderDevice->activeShader(mShader);
		glBindVertexArray(mVAO);

		glDrawElements(GL_TRIANGLES, mGridSize * mGridSize, GL_UNSIGNED_INT, 0);
	}

private:
	int mGridSize;
	GLShader* mShader;
	unsigned int mVBO, mVAO, mEBO;

	void initShader() {
		SShaderManager* shaderManager = SShaderManager::instance();

		vector<ShaderCode*> vsSources;
		vector<ShaderCode*> fsSources;
		string errorLog;

		ShaderCode* vertexShader =
			shaderManager->createShaderCode("Shader/Ocean.vert", "330 core",
				0, "", "");
		ShaderCode* fragmentShader =
			shaderManager->createShaderCode("Shader/Ocean.frag", "330 core",
				0, "", "");

		vsSources.push_back(vertexShader);
		fsSources.push_back(fragmentShader);

		mShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);

		shaderManager->release();
	}

	void initRenderObjects() {
		vector<float> vertices;
		for (int x = 0; x < mGridSize; x++) {
			for (int z = 0; z < mGridSize; z++) {
				vertices.push_back(-0.5f + 1.0f / x * mGridSize);
				vertices.push_back(0);
				vertices.push_back(-0.5f + 1.0f / z * mGridSize);
			}
		}

		vector<unsigned int> indices;
		for (int x = 1; x < mGridSize; x++) {
			for (int z = 1; z < mGridSize; z++) {
				int i = x * mGridSize + z;
				indices.push_back(i);
				indices.push_back(i - mGridSize);
				indices.push_back(i - mGridSize - 1);

				indices.push_back(i);
				indices.push_back(i - mGridSize - 1);
				indices.push_back(i - 1);
			}
		}

		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glGenBuffers(1, &mEBO);

		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}
};