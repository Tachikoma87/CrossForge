#pragma once

#include <iostream>
#include <random>
#include <math.h>
#include <algorithm>

#include "CForge/Core/CoreUtility.hpp"

using namespace Eigen;


	class CellularNoise3D{

	public:
		std::vector<Vector3f> points;
		float numCells;
		float maxDist;

		CellularNoise3D(int numCells, float width, float height, float depth) {

			this->numCells = numCells;
			maxDist = std::max(std::max(width, height), depth) * 2;

			//seed f�r randomF
			srand(time(NULL));

			// generate Points
			for (float i = 0; i < numCells; i++) {
				points.push_back(Vector3f(randomF(width / -2, width / 2), randomF(height / -2, height / 2), randomF(depth / -2, depth / 2)));
			}
		}

		float getValue(Vector3f point) {
			return getValue(point, 0, 1);
		}

		float getValue(Vector3f point, float min, float max) {

			std::vector<float> distances;
			for (int i = 0; i < points.size(); i++) {
				
				distances.push_back((points[i] - point).norm());
			}
			std::sort(distances.begin(), distances.end());
			
			
			float ret = distances[0] / maxDist;
			if (ret < maxDist / 100.0) {
				ret = 0.2;
			}
			else if (ret > 1) {
				ret = 1;
			}

			return (ret * (max - min) + min);
		}

	private:
		float randomF(float min, float max) {
			return (min + (float) rand() / (float) (RAND_MAX) * (max - min));
		}
	};


