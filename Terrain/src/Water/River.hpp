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

#include <algorithm>
#include <glad/glad.h>
#include <set>
#include <iostream>

using namespace CForge;
using namespace std;

struct streamMapPoint {
	int riverIndex = -1;
	float t = -1;
};

class River {
public:

	River(Vector2i dimension, float* heightMap, float* poolMap, streamMapPoint* streamMap, float height) {
		mHeightMap = heightMap;
		mPoolMap = poolMap;
		mStreamMap = streamMap;
		mDimension = dimension;
		mHeightScale = height;
	}

	
	void init(vector<Vector3d> *samplePoints, int resolution, float startWidth, float endWidth) {
		mResolution = resolution;

		vector<double> t;
		vector<double> x;
		vector<double> y;
		vector<double> z;

		for (auto point : *samplePoints) {
			int i = t.size();
			float mix = (float)i / (samplePoints->size() - 1);

			t.push_back(i);
			x.push_back(point.x());
			y.push_back(point.y());
			z.push_back(point.z());
			w.push_back((double)((1 - mix) * startWidth + mix * endWidth));
		}

		adjustHeightList(&y);

		mSplineX.set_points(t, x);
		mSplineY.set_points(t, y);
		mSplineZ.set_points(t, z);
		

		mLength = t.size();
	}

	void adjustHeightMap(set<int>* alreadyAdjusted) {
		
		set<int> thisRiverAlreadyTried;

		for (float i = 0; i < mLength - 1.0f; i += 0.5f / mResolution) {
			Vector3f middlePoint((float)mSplineX((double)i), (float)mSplineY((double)i), (float)mSplineZ((double)i));

			for (float xOffset = -getWidth(i) * 2.0f; xOffset < getWidth(i) * 2.0f; xOffset += 0.5f) {
				for (float zOffset = 0; zOffset < getWidth(i) / 2 / mResolution; zOffset += 0.5f) {
					Vector3f offsetPoint = middlePoint + getNormal(i) * xOffset + getTangent(i) * zOffset;;

					int index = posToIndex(offsetPoint.x(), offsetPoint.z());


					float newHeight = middlePoint.y() - riverDepth(xOffset, getWidth(i) * 1.0f, mDepth);
					float heightAdjsutment = (newHeight - mHeightMap[index]) * heightMapMixFactor(xOffset, getWidth(i) * 1.2f);

					if (alreadyAdjusted->insert(index).second || (heightAdjsutment < 0 && thisRiverAlreadyTried.insert(index).second)) {

						mHeightMap[index] += heightAdjsutment;
						mPoolMap[index] -= mPoolMap[index] > 0 ? heightAdjsutment : 0;
					}
				}
			}
		}
	}

	void addWidthFrom(int startIndex, float width) {
		for (int i = startIndex; i < w.size(); i++) {
			w.at(i) += width;
		}

	
	}

	float getFoamFactor(double t, float height) {
		double foamLength = 2;
		//äsdjgfosdjfojsdofjdsojfsdojfiosdjfjsodjfojosjdofjoisijdfosdj
		return 0;
	}

	void setLength(float newLength) {
		mLength = newLength > mLength ? mLength : newLength;
	}

	int getLength() {
		return mLength;
	}

	float getRiverDepth() {
		return mDepth;
	}

	float getWidth(double t) {
		t = t < 0 ? 0 : t;
		t = t > (w.size() - 1) ? (w.size() - 1) : t;
		return w.at((int)t);
	}

	Vector3f getPos(double t) {
		return Vector3f((float)mSplineX(t) - mDimension.x() / 2.0f, (float)mSplineY(t) * mHeightScale, (float)mSplineZ(t) - mDimension.y() / 2.0f);
	}

	Vector3f getTangent(double t) {
		return Vector3f((float)mSplineX.deriv(1, t), 0, (float)mSplineZ.deriv(1, t)).normalized();
	}

	Vector3f getNormal(double t) {
		return Vector3f((float)mSplineZ.deriv(1, t), 0, -(float)mSplineX.deriv(1, t)).normalized();
	}
	Vector3f get3DNormal(double t) {
		return get3DTangent(t).cross(getNormal(t));
	}

	Vector3f get3DTangent(double t) {
		return Vector3f((float)mSplineX.deriv(1, t), (float)mSplineY.deriv(1, t) * mHeightScale, (float)mSplineZ.deriv(1, t)).normalized();
	}

	int getResolution() {
		return mResolution;
	}

private:
	float mDepth = lowQuality ? 0.04 : 0.02;

	int mResolution = 1;

	float mLength = 0;

	float* mHeightMap;
	float* mPoolMap;
	streamMapPoint* mStreamMap;
	Vector2i mDimension;
	float mHeightScale;

	
	vector<double> w;

	tk::spline mSplineX;
	tk::spline mSplineY;
	tk::spline mSplineZ;

	void adjustHeightList(vector<double>* heightList) {
		

		
		for (int i = 1; i < heightList->size(); i++) {

			int numElementsBigger = 0;
			for (int j = i; j < heightList->size() && heightList->at(j) > heightList->at(i - 1); j++) {
				numElementsBigger++;
			}

			if (numElementsBigger > 0) {
				int firstIndex = i - 1;
				int lastIndex = i + numElementsBigger + 1;
				float startHeight = heightList->at(firstIndex);
				if (lastIndex >= heightList->size()) lastIndex = heightList->size() - 1;
				float endHeight = heightList->at(lastIndex);


				for (int j = firstIndex; j <= lastIndex; j++) {
					float mixFactor = (j - firstIndex) / (float)(lastIndex - firstIndex);

					heightList->at(j) = startHeight * (1 - mixFactor) + endHeight * mixFactor;
				}
			}
		}

		for (int i = 0; i < heightList->size(); i++) {
			heightList->at(i) -= 0.002;
		}
		
	}



	


	float riverDepth(float x, float width, float depth) {
		if (abs(x) > width / 2.0f) return 0.0f;
		return (1 - pow(abs(x * 2 / width), 2)) * depth;
	}

	float heightMapMixFactor(float x, float width) {
		if (abs(x) < width / 2.0f) return 1;

		return max(1.0f - abs(x / width), 0.0f);
	}

	int posToIndex(float x, float y) {
		x += 0.5f;
		y += 0.5f;
		return ((int)x * mDimension.y() + (int)y);
	}

};