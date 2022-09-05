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
		int maxDist = lowQuality ? 6 : 10;

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
			
			simulateDroplet(pos, lowQuality ? 3 : 7.0);
			
		}
		
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
		int maxDist = lowQuality ? 12 : 50;
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

		
		int res = lowQuality ? 1 : 3;

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
	int mMaxLakeSize = 300000;
	int mMinLakeSize = 1000;
	vector<int> mOffsets;

	Vector3f getNormal(Vector2f pos) {
		if (pos.x() < 2 || pos.x() >= mDimension.x() - 2 || pos.y() < 2 || pos.y() >= mDimension.y() - 2) return Vector3f(0, 1, 0);

		int index = (int)pos.x() * mDimension.y() + (int)pos.y();
		float a = mHeightMap[index - mDimension.y()];
		float b = mHeightMap[index + mDimension.y()];
		float c = mHeightMap[index - 1];
		float d = mHeightMap[index + 1];
		Vector3f ret = Vector3f(a - b, 1, c - d);
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



	void simulateDroplet(Vector2f pos, float startWidth) {
		int maxLoopCycles = 2000;
		int minLoopCycles = 50;

		int riverResolution = 5;

		float curveFactor = 0.03;

		set<int> river;
		vector<int> riverVec;
		Vector3f n = getNormal(pos);
		Vector2f speed = Vector2f(n.x(), n.z()).normalized();
		int index = (int)pos.x() * mDimension.y() + (int)pos.y();

		float startHeight = mHeightMap[index];

		river.insert(index);
		riverVec.push_back(index);

		int lastIndex = index;

		vector<Vector3d> riverPoints;

		float endWidth = 0;

		float endHeightAdjustment = 0;

		float lowestHeight = 999999;

		for (int i = 0; i < maxLoopCycles; i++) {
			

			if (mStreamMap[index].riverIndex != -1) {
				if (endWidth > mRivers[mStreamMap[index].riverIndex].getWidth(mStreamMap[index].t)) {
					mRivers[mStreamMap[index].riverIndex].setLength(mStreamMap[index].t + 2);
					delStreamMapRiverEnd(mStreamMap[index].riverIndex, mStreamMap[index].t);
				}
				else {
					//mRivers[mStreamMap[index].riverIndex].addWidthFrom(mStreamMap[index].t, endWidth / 4);

					//endHeightAdjustment = riverPoints[riverPoints.size() - 1].y() - mRivers[mStreamMap[index].riverIndex].getPos(mStreamMap[index].t).y();
					pos = pos + speed * mRivers[mStreamMap[index].riverIndex].getWidth(mStreamMap[index].t) / 2;
					index = (int)pos.x() * mDimension.y() + (int)pos.y();
					riverPoints.push_back(Vector3d((double)pos.x(), (double)mHeightMap[index], (double)pos.y()));

					break;
				}
				
			}

			if (mPoolMap[index] > 0.005 || mHeightMap[index] < 0.45) {
				break;
			}

			n = getNormal(pos);
			
			speed = (speed * curveFactor + Vector2f(n.x(), n.z()) * (1 - curveFactor)).normalized();
			pos = pos + speed;
			index = (int)pos.x() * mDimension.y() + (int)pos.y();

			if (lowestHeight < mHeightMap[index] - 0.03) {
				return;
			}
			lowestHeight = lowestHeight < mHeightMap[index] ? lowestHeight : mHeightMap[index];

			if (!river.insert(index).second && index != lastIndex) {
				return;
				generateLakeAtIndex(index);
				break;
			}
			riverVec.push_back(index);

			if (i % riverResolution == 0) {
				riverPoints.push_back(Vector3d((double)pos.x(), (double)mHeightMap[index], (double)pos.y()));
			}
			endWidth = (50.0f * river.size() / maxLoopCycles) + startWidth;

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

			mRivers.back().init(&riverPoints, riverResolution, startWidth, endWidth);


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
};


