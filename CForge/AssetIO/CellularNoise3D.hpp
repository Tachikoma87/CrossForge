#include <iostream>
#include <random>
#include <math.h>
#include <algorithm>

#include "../Core/CoreUtility.hpp"

using namespace Eigen;

namespace CForge {

	class CellularNoise3D{

	public:
		std::vector<Vector3f> points;
		float cellSize;

		CellularNoise3D(float cellSize, float width, float height, float depth) {

			this->cellSize = cellSize;

			//seed für randomF
			srand(time(NULL));

			// generate Points
			for (float x = width / -2; x < width / 2; x += cellSize) {
				for (float y = height / -2; y < height; y += cellSize) {
					for (float z = depth / -2; z < depth; z += cellSize) {
						points.push_back(Vector3f(randomF(x, x + cellSize),
												  randomF(y, y + cellSize),
												  randomF(z, z + cellSize)));
					}
				}
			}		
		}

		float getValue(Vector3f point) {
			return getValue(point, 0, 1);
		}

		float getValue(Vector3f point, float min, float max) {

			float minDist = cellSize;
			for (int i = 0; i < points.size(); i++) {
				minDist = std::min(minDist, (point - points[i]).norm());
				//printf("%f\n", (point - points[i]).norm());
				//printf("(%f, %f, %f)\n", point.x(), point.y(), point.z());
				//printf("(%f, %f, %f)\n", points[i].x(), points[i].y(), points[i].z());
			}
			
			return (minDist / cellSize * (max - min) + min);
		}

	private:
		float randomF(float min, float max) {
			return (min + (float) rand() / (float) (RAND_MAX) * (max - min));
		}
	};
}

