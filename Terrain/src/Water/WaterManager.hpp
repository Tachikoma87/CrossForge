#pragma once

#include "CForge/Core/CoreUtility.hpp"
#include "CForge/Graphics/GraphicsUtility.h"

#include "../../../CForge/Graphics/RenderDevice.h"

#include "CForge/Graphics/STextureManager.h"

#include "../../../CForge/Graphics/Shader/SShaderManager.h"

#include "Terrain/src/Map/TerrainMap.h"


#include "../Decoration/PerlinNoise.hpp"

#include "spline.h"
#include "Settings.hpp"

#include <glad/glad.h>
#include <set>
#include <stack>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>


using namespace CForge;
using namespace std;




class WaterManager {
public:

	WaterManager(Terrain::HeightMap* hMap) {
		mHeightMapObject = hMap;
		mDimension = Vector2i(hMap->getConfig().width, hMap->getConfig().height);
		mHeightMap = vector<float>(mDimension.x() * mDimension.y(), 0.0f);
		mPoolMap = vector<float>(mDimension.x() * mDimension.y(), 0.0f);
		mPoolMapScaled = vector<float>(mDimension.x() * mDimension.y(), 0.0f);
		mStreamMap = vector<streamMapPoint>(mDimension.x() * mDimension.y());

		mOffsets = { -mDimension.y(), -1 , 1, mDimension.y() };

		initTextures();

		//generateHeightMapValues();

	}


	bool validTreeLocation(float x, float y) {
		bool ret = true;
		int index = (int)x * mDimension.y() + (int)y;
		int maxDist = 10;

		if (x - 1 < maxDist || x + 1 > mDimension.x() - maxDist ||
			y - 1 < maxDist || y + 1 > mDimension.y() - maxDist) return false;

		for (int d = 1; d < maxDist; d++) {
			for (auto o : mOffsets) {
				int offsetIndex = index + o * d;
				ret = ret && (mStreamMap[offsetIndex].riverIndex == -1) && (mPoolMap[offsetIndex] == 0);
			}
		}


		return ret;
	}

	void trySpawnLakes(int res) {
		fill(mPoolMap.begin(), mPoolMap.end(), 0);

		for (int y = res / 2; y < mDimension.y(); y += res) {
			for (int x =  res / 2; x < mDimension.x(); x += res) {
				int index = x * mDimension.y() + y;
				index = findValley(index);

				if (index == -1) continue;
				if (generateLakeAtIndex(index))	mPoolPositions.push_back(Vector2f(x, y));
			}
		}

		/*

		int lakeCounter = 0;
		int attempts = 0;

		while (lakeCounter < amount && attempts < 100) {
			Vector2f pos = Vector2f(rand() % (mDimension.x() - 1), rand() % (mDimension.y() - 1));
			int index = pos.x() * mDimension.y() + pos.y();

			if (generateLakeAtIndex(index)) {
				lakeCounter++;

				mPoolPositions.push_back(pos);
			}
			attempts++;

		}

		*/
	}

	void tryGenerateRivers(int ammount) {
		//findDrains();
		
		for (int i = 0; i < ammount; i++) {
		
			Vector2f pos = Vector2f(randomF(2, mDimension.x() - 2), randomF(2, mDimension.y() - 2));
			
			Vector3f n = getNormal(pos);
			Vector2f speed = Vector2f(n.x(), n.z()).normalized();

			simulateDroplet(pos, mRiverStartWidth, speed);
			
		}

		deleteFalseRivers();
		deleteFalseRivers();
		deleteFalseRivers();

		
		set<int> alreadyAdjusted;
		for (auto r : mRivers) {
			r.adjustHeightMap(&alreadyAdjusted);
		}
	}



	void generateHeightMapValues() {
		siv::PerlinNoise pNoise;
		pNoise.reseed(12);
		float scale = 0.04;
		for (int y = 0; y < mDimension.y(); y++) {
			for (int x = 0; x < mDimension.x(); x++) {
				mHeightMap[x * mDimension.y() + y] = pNoise.accumulatedOctaveNoise2D(x * scale, y * scale, 10) * 0.04f
												   + pNoise.accumulatedOctaveNoise2D(x * scale / 10.0, y * scale / 10.0, 2) * 0.2 + 0.65f;
			}
		}
		
	}

	void refreshMaps() {
		glBindTexture(GL_TEXTURE_2D, mHeightMapObject->getTextureHandle());
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, mHeightMap.data());
		fill(mPoolMap.begin(), mPoolMap.end(), 0);
		fill(mStreamMap.begin(), mStreamMap.end(), streamMapPoint());
		mRivers.clear();
	}
	

	void initTextures() {
		refreshMaps();

		glGenTextures(1, &mPoolMapTextureHandle);
		glBindTexture(GL_TEXTURE_2D, mPoolMapTextureHandle);
		glTextureParameteri(mPoolMapTextureHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(mPoolMapTextureHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mDimension.x(), mDimension.y(), 0, GL_RED, GL_FLOAT, mPoolMapScaled.data());

		glGenTextures(1, &mShoreDistanceTextureHandle);
		glBindTexture(GL_TEXTURE_2D, mShoreDistanceTextureHandle);
		glTextureParameteri(mShoreDistanceTextureHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mShoreDistanceTextureHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mDimension.x(), mDimension.y(), 0, GL_RED, GL_FLOAT, NULL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mDimension.x(), mDimension.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	void updateTextures() {

		for (int x = 0; x < mDimension.x(); x++) {
			for (int y = 0; y < mDimension.y(); y++) {
				int index = x * mDimension.y() + y;
				if (mPoolMap[index > 0 || mStreamMap[index].riverIndex != -1]) mPoolMapScaled[index] = 1;
				else mPoolMapScaled[index] = 0;
			}
		}

		glBindTexture(GL_TEXTURE_2D, mHeightMapObject->getTextureHandle());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mDimension.x(), mDimension.y(), 0, GL_RED, GL_FLOAT, mHeightMap.data());

		glBindTexture(GL_TEXTURE_2D, mPoolMapTextureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mDimension.x(), mDimension.y(), 0, GL_RGBA, GL_FLOAT, NULL);
	}

	unsigned int getHeightMapTexture() {
		return mHeightMapObject->getTextureHandle();
	}

	unsigned int getPoolMapTexture() {
		return mPoolMapTextureHandle;
	}

	unsigned int getShoreDistanceTexture() {
		return mShoreDistanceTextureHandle;
	}

	float* getHeightMap() {
		return mHeightMap.data();
	}

	float* getPoolMap() {
		return mPoolMap.data();
	}

	streamMapPoint* getStreamMap() {
		return mStreamMap.data();
	}

	vector<River>* getRivers() {
		return &mRivers;
	}

	void updateShoreDistTexture() {
		int maxDist = 10 * gSettings.sizeScale;
		vector<float> shoreDistances(mDimension.x() * mDimension.y() * 4, maxDist);

		for (int i = 0; i < shoreDistances.size(); i += 4) {
			if (mHeightMap[i / 4] > 0.5) shoreDistances[i] = 0;
			//Vector3f n = getNormal(indexToPos(i / 4));
			Vector2f p = indexToPos(i / 4) - Vector2f(mDimension.x() / 2.0f, mDimension.y()/ 2.0f);
			p.normalize();
			shoreDistances[i + 1] = -p.x();// n.x();
			shoreDistances[i + 2] = -p.y();// n.y();
			shoreDistances[i + 3] = 0;// n.z();
		}

		/*
		for (int d = 1; d < maxDist; d++) {
			for (int i = 0; i < shoreDistances.size(); i++) {
				if (shoreDistances[i] > d) {
					for (auto o : mOffsets) {
						int offsetIndex = i + o;
						if (isInBounds(offsetIndex)) {
							if (shoreDistances[offsetIndex] < d) {
								shoreDistances[i] = d;
							}
						}
					}
				}
			}
		}
		*/

		
		int res = gSettings.sizeScale / 2 + 1;

		for (int x = 0; x < mDimension.x(); x += res) {
			for (int y = 0; y < mDimension.y(); y += res) {
				int i = posToIndex(x, y);


				bool isShore = false;
				if (mHeightMap[i] > 0.5) {
					for (auto o : mOffsets) {
						int offsetIndex = i + o;
						if (isInBounds(offsetIndex)) {
							isShore = isShore || mHeightMap[offsetIndex] < 0.5;
						}
					}
				}


				if (isShore) {
					Vector2f pos = indexToPos(i);
					for (int xOffset = -maxDist; xOffset <= maxDist; xOffset++) {
						for (int yOffset = -maxDist; yOffset <= maxDist; yOffset++) {
							Vector2f offsetPos(pos.x() + xOffset, pos.y() + yOffset);
							if (isInBounds(offsetPos)) {
								float distance = sqrtf(xOffset * xOffset + yOffset * yOffset);
								int offsetIndex = posToIndex(offsetPos.x(), offsetPos.y()) * 4;

								if (shoreDistances[offsetIndex] > distance) {
									shoreDistances[offsetIndex] = distance;
									Vector2f o(-xOffset, -yOffset);
									o.normalize();
									shoreDistances[offsetIndex + 1] = o.x();
									shoreDistances[offsetIndex + 2] = o.y();
									shoreDistances[offsetIndex + 3] = 0;
								}
							}
						}
					}
				}
			}
		}


		for (int i = 0; i < shoreDistances.size(); i += 4) {
			shoreDistances[i] /= maxDist;
		}

		glBindTexture(GL_TEXTURE_2D, mShoreDistanceTextureHandle);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mDimension.x(), mDimension.y(), 0, GL_RED, GL_FLOAT, shoreDistances.data());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mDimension.x(), mDimension.y(), 0, GL_RGBA, GL_FLOAT, shoreDistances.data());
	}

	
	void exportMaps() {
		{
			std::ofstream outfile("Assets/heightMap.txt", ios::out | ios::binary);
			outfile.write((char*)(mHeightMap.data()), mHeightMap.size() * sizeof(float));
			outfile.close();
		}
		{
			std::ofstream outfile("Assets/poolMap.txt", ios::out | ios::binary);
			outfile.write((char*)(mPoolMap.data()), mPoolMap.size() * sizeof(float));
			outfile.close();
		}
		{
			std::ofstream outfile("Assets/streamMap.txt", ios::out | ios::binary);
			outfile.write((char*)(mStreamMap.data()), mStreamMap.size() * sizeof(streamMapPoint));
			outfile.close();
		}
		{
			std::ofstream outfile("Assets/riverObjects.txt", ios::out | ios::trunc);
			outfile << mRivers.size() << endl;
			for (auto river : mRivers) {
				outfile << river.getLength() << " " << river.getResolution() << endl;
				vector<double>* x = river.getXVector();
				vector<double>* y = river.getYVector();
				vector<double>* z = river.getZVector();
				vector<double>* w = river.getWVector();
				for (int i = 0; i < river.getLength(); i++) {
					outfile << x->at(i) << " " << y->at(i) << " " << z->at(i) << " " << w->at(i) << endl;
				}
			}
			outfile.close();
		}
	}

	void importMaps() {
		{
			std::ifstream infile("Assets/heightMap.txt", ios::in | ios::binary);
			infile.seekg(0, std::ios::end);
			size_t length = infile.tellg();
			infile.seekg(0, std::ios::beg);
			if (length > sizeof(float) * mDimension.x() * mDimension.y()) length = sizeof(float) * mDimension.x() * mDimension.y();
			infile.read((char*)mHeightMap.data(), length);
			infile.close();
		}
		{
			std::ifstream infile("Assets/poolMap.txt", ios::in | ios::binary);
			infile.seekg(0, std::ios::end);
			size_t length = infile.tellg();
			infile.seekg(0, std::ios::beg);
			if (length > sizeof(float) * mDimension.x() * mDimension.y()) length = sizeof(float) * mDimension.x() * mDimension.y();
			infile.read((char*)mPoolMap.data(), length);
			infile.close();
		}
		{
			std::ifstream infile("Assets/streamMap.txt", ios::in | ios::binary);
			infile.seekg(0, std::ios::end);
			size_t length = infile.tellg();
			infile.seekg(0, std::ios::beg);
			if (length > sizeof(streamMapPoint) * mDimension.x() * mDimension.y()) length = sizeof(streamMapPoint) * mDimension.x() * mDimension.y();
			infile.read((char*)mStreamMap.data(), length);
			infile.close();
		}
		{
			mRivers.clear();
			std::ifstream infile("Assets/riverObjects.txt");
			int numberRivers, numberRiverPoints, res;
			infile >> numberRivers;
			for (int i = 0; i < numberRivers; i++) {
				vector<double> xVec, yVec, zVec, wVec;
				double x, y, z, w;

				infile >> numberRiverPoints >> res;
				for (int j = 0; j < numberRiverPoints; j++) {
					infile >> x >> y >> z >> w;
					xVec.push_back(x);
					yVec.push_back(y);
					zVec.push_back(z);
					wVec.push_back(w);
				}

				if (numberRiverPoints < 3) continue;
				mRivers.push_back(River(mDimension, mHeightMap.data(), mPoolMap.data(), mStreamMap.data(), mHeightMapObject->getConfig().mapHeight));
				mRivers.back().initFromSave(&xVec, &yVec, &zVec, &wVec, res);
			}
			infile.close();
		}
		updateTextures();
	}
	

private:
	bool activeErosion = false;
	Terrain::HeightMap* mHeightMapObject;
	Vector2i mDimension;
	vector<float> mHeightMap;
	vector<float> mPoolMap;
	vector<float> mPoolMapScaled;
	vector<streamMapPoint> mStreamMap;
	vector<River> mRivers;
	float mFadeRate = 0.05;
	unsigned int mPoolMapTextureHandle, mShoreDistanceTextureHandle;
	vector<Vector2f> mPoolPositions;
	int mMaxLakeSize = pow(120 * gSettings.sizeScale, 2);
	int mMinLakeSize = pow(8 * gSettings.sizeScale, 2);
	vector<int> mOffsets;
	float mRiverStartWidth = 4;

	Vector3f getNormal(Vector2f pos) {
		if (pos.x() < 2 || pos.x() >= mDimension.x() - 2 || pos.y() < 2 || pos.y() >= mDimension.y() - 2) return Vector3f(0, 1, 0);

		int index = (int)pos.x() * mDimension.y() + (int)pos.y();
		float a = mHeightMap[index - mDimension.y()];
		float b = mHeightMap[index + mDimension.y()];
		float c = mHeightMap[index - 1];
		float d = mHeightMap[index + 1];
		Vector3f ret = Vector3f(a - b, 1 * mHeightMapObject->getConfig().mapHeight, c - d);
		ret.normalize();
		return ret;
	}

	int posToIndex(float x, float y) {
		return ((int)x * mDimension.y() + (int)y);
	}

	Vector2f indexToPos(int index) {
		return Vector2f((int)(index / mDimension.y()), index % mDimension.y());
	}

	bool isInBounds(int index) {
		return (index > 0 && index < mDimension.x()* mDimension.y());
	}

	bool isInBounds(Vector2f pos) {
		return (pos.x() > 0 && pos.x() < mDimension.x()) && (pos.y() > 0 && pos.y() < mDimension.y());
	}
	
	bool generateLakeAtIndex(int index) {
		float resolution = 0.003f;

		vector<bool> triedTiles(mDimension.x() * mDimension.y(), false);
		vector<bool> lastConfirmedLake(mDimension.x() * mDimension.y(), false);
		int lakeSize = 0;
		float height = mHeightMap[index] + resolution;

		bool succ = false;


		while (tryFill(index, height, &triedTiles, &lakeSize)) {
			lastConfirmedLake = triedTiles;
			fill(triedTiles.begin(), triedTiles.end(), false);

			succ = lakeSize > mMinLakeSize;

			lakeSize = 0;
			height += resolution;
			
			
		}

		height -= resolution;

		if (succ) {
			for (int x = 0; x < mDimension.x(); x++) {
				for (int y = 0; y < mDimension.y(); y++) {
					int i = x * mDimension.y() + y;
					if (lastConfirmedLake[i]) { 
						mPoolMap[i] = height - mHeightMap[i];
						mStreamMap[i] = streamMapPoint();
					}
				}
			}
		}

		return succ;
	
	}

	bool tryFill(int index, float height, vector<bool>* triedTiles, int *lakeSize) {
		stack<int> indexStack;
		indexStack.push(index);
		while (!indexStack.empty()) {
			int i = indexStack.top();
			indexStack.pop();

			if (*lakeSize > mMaxLakeSize || mHeightMap[i] < 0.5 || mPoolMap[i] > 0) return false;
			
			if (!triedTiles->at(i) && mHeightMap[i] < height) {
				triedTiles->at(i) = true;
				*lakeSize += 1;

				for (auto o : mOffsets) {
					indexStack.push(i + o);
				}
			}
		}
		return true;


		/*
		triedTiles->at(index) = true;
		*lakeSize += 1;

		if (*lakeSize > mMaxLakeSize) return false;
		if (mHeightMap[index] < 0.5) return false;
		if (mPoolMap[index] > 0) return false;

		bool ret = true;

		
		for(auto o : mOffsets) {
			int offsetIndex = index + o;
			if (offsetIndex >= 0 && offsetIndex < triedTiles->size()) {

				if (!triedTiles->at(offsetIndex) && mHeightMap[offsetIndex] < height) {
					ret = ret && tryFill(offsetIndex, height, triedTiles, lakeSize);
				}
			}
		}
		return ret;
		*/
	}

	int findValley(int startIndex) {
		int maxDist = 3;
		bool moved = true;
		int moveIndex = startIndex;

		while (moved) {	
			for (int d = 1; d <= maxDist; d++) {
				for (auto o : mOffsets) {
					int offsetIndex = startIndex + o * d;

					if (offsetIndex < 0 || offsetIndex >= mDimension.x() * mDimension.y()) return -1;

					if (mHeightMap[moveIndex] > mHeightMap[offsetIndex]) moveIndex = offsetIndex;
				}
			}

			moved = (moveIndex != startIndex);
			startIndex = moveIndex;
			if (mHeightMap[startIndex] < 0.5 || mPoolMap[startIndex] > 0) return -1;
		}

		return startIndex;
	}

	void findDrains() {
		int riverChance = 1000; //lower --> more rivers

		for (int x = 1; x < mDimension.x() - 1; x++) {
			for (int y = 1; y < mDimension.y() - 1; y++) {
				int i = x * mDimension.y() + y;
				
				if (mPoolMap[i] == 0 && mHeightMap[i] > 0.5f) {
					
					bool createRiver = false;

					for (auto o : mOffsets) {
						int offsetIndex = i + o;
						if (mPoolMap[offsetIndex] > 0 || mHeightMap[offsetIndex] < 0.5f) {
							createRiver = randomF(0, riverChance) < 1;
						}
					}
					

					if (createRiver) {

						//genRiverFromPos(Vector2f(x, y), 8, 0, 2000);
					}
				
				}
			}
		}
	}



	void simulateDroplet(Vector2f pos, float startWidth, Vector2f startSpeed) {
		int riverResolution = 3;
		int maxLoopCycles = 2000 * gSettings.sizeScale;
		int minLoopCycles = riverResolution * 4 * (int)sqrt(gSettings.sizeScale);

		

		

		set<int> river;
		vector<int> riverVec;
		Vector3f n = getNormal(pos);
		Vector2f speed = startSpeed;
		int index = (int)pos.x() * mDimension.y() + (int)pos.y();

		vector<int> addWidthIndex;
		vector<float> widthToAdd;

		float startHeight = mHeightMap[index];

		river.insert(index);
		riverVec.push_back(index);

		int lastIndex = index;

		vector<Vector3d> riverPoints;

		float endWidth = startWidth;
		float curveFactor;
		float endHeightAdjustment = 0;

		float lowestHeight = 999999;

		for (int i = 0; i < maxLoopCycles; i++) {
			

			if (mStreamMap[index].riverIndex != -1) {
				if (i < minLoopCycles) return;
				int collidedRiverIndex = mStreamMap[index].riverIndex;
				float collidedRiverWidth = mRivers[collidedRiverIndex].getWidth(mStreamMap[index].t);
				float widthAddFactor = 0.45;
				/*
				if (endWidth > mRivers[mStreamMap[index].riverIndex].getWidth(mStreamMap[index].t)) {

					addWidthIndex.push_back(riverPoints.size() - 1);
					widthToAdd.push_back(collidedRiverWidth * widthAddFactor);
					//startWidth += collidedRiverWidth * widthAddFactor;
					endWidth += collidedRiverWidth * widthAddFactor;

					mRivers[collidedRiverIndex].setLength(mStreamMap[index].t + 2);
					delStreamMapRiverEnd(collidedRiverIndex, mStreamMap[index].t);
				}
				else {
					mRivers[collidedRiverIndex].addWidthFrom(mStreamMap[index].t, endWidth * widthAddFactor);
					//mRivers[collidedRiverIndex].addWidthFrom(0, endWidth * widthAddFactor);

					//endHeightAdjustment = riverPoints[riverPoints.size() - 1].y() - mRivers[mStreamMap[index].riverIndex].getPos(mStreamMap[index].t).y();
					Vector3f endPos = mRivers[collidedRiverIndex].getMapPos((double)mStreamMap[index].t);
					riverPoints.push_back(Vector3d((double)endPos.x(), (double)endPos.y(), (double)endPos.z()));

					break;
				}
				*/
				
				float newWidth = sqrtf(collidedRiverWidth * collidedRiverWidth + endWidth * endWidth); //collidedRiverWidth > endWidth ? (collidedRiverWidth + endWidth * widthAddFactor) : (endWidth + collidedRiverWidth * widthAddFactor);
				Vector3f collidedRiverSpeed3 = mRivers[collidedRiverIndex].getMapPos(mStreamMap[index].t + 1) - mRivers[collidedRiverIndex].getMapPos(mStreamMap[index].t);
				Vector2f collidedRiverSpeed2 = Vector2f(collidedRiverSpeed3.x(), collidedRiverSpeed3.z()).normalized();

				speed = (speed * endWidth + collidedRiverSpeed2 * collidedRiverWidth).normalized();

				addWidthIndex.push_back(riverPoints.size() - 1);
				widthToAdd.push_back(newWidth - endWidth);
				endWidth = newWidth;
				mRivers[collidedRiverIndex].setLength(mStreamMap[index].t + 2);
				delStreamMapRiverEnd(collidedRiverIndex, mStreamMap[index].t);
				
			}

			if (mPoolMap[index] > 0.005 || mHeightMap[index] < 0.49) {
				riverPoints.push_back(Vector3d((double)pos.x(), (double)mHeightMap[index], (double)pos.y()));
				break;
			}

			n = getNormal(pos);
			curveFactor = 0.0002 / gSettings.sizeScale / gSettings.sizeScale * endWidth;
			speed = (speed * curveFactor + Vector2f(n.x(), n.z()) * (1 - curveFactor)).normalized();
			pos = pos + speed;
			index = (int)pos.x() * mDimension.y() + (int)pos.y();

			if (lowestHeight < mHeightMap[index] - 0.01) {
				return;
			}

			lowestHeight = lowestHeight < mHeightMap[index] ? lowestHeight : mHeightMap[index];

			if (!river.insert(index).second && index != lastIndex) return;
			riverVec.push_back(index);

			if (i % riverResolution == 0) {
				riverPoints.push_back(Vector3d((double)pos.x(), (double)mHeightMap[index], (double)pos.y()));
			}
			

			lastIndex = index;
		}		

		for (int i = 0; i < riverPoints.size(); i++) {
			riverPoints[i].y() += endHeightAdjustment * i / (riverPoints.size() - 1);
		}

		if (mHeightMap[index] + mPoolMap[index] > startHeight) {
		
			return;
		}
		if (river.size() >= minLoopCycles) {
			mRivers.push_back(River(mDimension, mHeightMap.data(), mPoolMap.data(), mStreamMap.data(), mHeightMapObject->getConfig().mapHeight));

			mRivers.back().init(&riverPoints, riverResolution, startWidth, startWidth);

			for (int j = 0; j < addWidthIndex.size(); j++) {
				mRivers.back().addWidthFrom(addWidthIndex[j], widthToAdd[j]);
			}
			

			for (int j = 0; j < riverVec.size(); j++) {
				int i = riverVec[j];
				for (auto o : mOffsets) {
					int offsetIndex = i + o;
					if (mPoolMap[offsetIndex] == 0) mStreamMap[offsetIndex] = { (int)mRivers.size() - 1, max((float)j / riverResolution - 1, 0.0f) };
				}
			}

			
		}
		
	}

	void delStreamMapRiverEnd(int riverIndex, int pointIndex) {
		for (int i = 0; i < mStreamMap.size(); i++) {
			if (mStreamMap[i].riverIndex == riverIndex && mStreamMap[i].t > pointIndex) {
				mStreamMap[i] = { -1, -1 };
			}
		}
	}

	bool checkRiverValidity(int riverID) {
		Vector3f mapPos = mRivers[riverID].getMapPos(mRivers[riverID].getLength() - 1);

		bool ret = false;
		int x = mapPos.x(), y = (int)mapPos.z();
		int index = x * mDimension.y() + y;
		int maxDist = 5;

		if (x - 1 < maxDist || x + 1 > mDimension.x() - maxDist ||
			y - 1 < maxDist || y + 1 > mDimension.y() - maxDist) return false;

		for (int d = 1; d < maxDist; d++) {
			for (auto o : mOffsets) {
				int offsetIndex = index + o * d;
				ret = ret || (mStreamMap[offsetIndex].riverIndex != -1) && (mStreamMap[offsetIndex].riverIndex != riverID);
			}
		}

		ret = ret || (mPoolMap[index] > 0) || ((mHeightMap[index] < 0.5) && mRivers[riverID].getWidth(mRivers[riverID].getLength() - 1) > mRiverStartWidth + 0.1);


		return ret;
	}

	bool deleteFalseRivers() {
		bool ret = false;

		for (int i = 0; i < mRivers.size(); i++) {
			if (!checkRiverValidity(i) && mRivers[i].getLength() > 0) {
				mRivers[i].setLength(0);
				delStreamMapRiverEnd(i, 0);
				ret = true;
			}
		}

		return true;
	}
};


