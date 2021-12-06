#ifndef __CFORGE_GRASS_GENERATOR_H__
#define __CFORGE_GRASS_GENERATOR_H__

#include "../Core/CoreUtility.hpp"
#include "ObjExporter.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#define PI 3.14159265

using namespace Eigen;
using namespace std;

enum GrassType {cross, triangle};

class TreeGenerator {
	public:
		static void generateGrass(GrassType grassType, int amount, string exportPath) {
			for (int i = 0; i < amount; i++) {
				// new random Seed
				srand(time(NULL));
				
				// grass generation 
				GEOMETRY geometry;


				switch (grassType) {
					case cross:
						generate(geometry, 0, 3);
						break;
					case triangle:
						generate(geometry, 2, 3);
						break;
				}

				ObjExporter::meshExport(geometry, exportPath + "grass" + to_string(i) + ".obj");
			}
		}

	protected:

		// generate grass
		static void generate(GEOMETRY& geometry, float radius, float size) {
			Vector3f bitangent = Vector3f(0, 1, 0);
			Vector3f normal = Vector3f(0, 0, 1);
			Vector3f tangent = bitangent.cross(normal);
			Matrix3f tangentSpace;
			tangentSpace.col(0) = Vector3f(0, 0, 1);
			tangentSpace.col(1) = Vector3f(1, 0, 0);
			tangentSpace.col(2) = Vector3f(0, 1, 0);
			
			geometry.uvs.push_back(Vector2f(0, 1));
			geometry.uvs.push_back(Vector2f(0, 0));
			geometry.uvs.push_back(Vector2f(1, 0));
			geometry.uvs.push_back(Vector2f(1, 1));

			for (int i = 0; i < 3; i++) {
				geometry.vertices.push_back(tangentSpace * Vector3f(size / -2, size, radius));
				geometry.vertices.push_back(tangentSpace * Vector3f(size / -2, 0, radius));
				geometry.vertices.push_back(tangentSpace * Vector3f(size / 2, 0, radius));
				geometry.vertices.push_back(tangentSpace * Vector3f(size / 2, size, radius));
				geometry.normals.push_back(tangentSpace.col(0));
				int lastIntex = geometry.vertices.size();
				geometry.faces.push_back({ Vector3i(lastIntex - 3, 1, 1), Vector3i(lastIntex - 2, 2, 1), Vector3i(lastIntex - 1, 3, 1), Vector3i(lastIntex, 4, 1) });

				float angle = i * 2 * PI / 3;
				tangentSpace.col(0) = Vector3f(cos(angle), 0, cos(angle));
				tangentSpace.col(1) = Vector3f(cos(angle), 0, -1 * sin(angle));
			}
		}


		static void generateCircleVertices(GEOMETRY& geometry, int pointsPerCircle, Vector3f centrePoint, float radius, Matrix3<float>& rotationMatrix, float uvCordU) {
			for (int i = 0; i <= pointsPerCircle; i++) {
				float angle = 2 * PI * ((float)i) / pointsPerCircle;
				Vector3f normal(cos(angle), 0, -1 * sin(angle));
				normal = rotationMatrix * normal;
				Vector3f verticePos = normal * radius + centrePoint;
				geometry.vertices.push_back(verticePos);
				geometry.uvs.push_back(Vector2f(((float)i) / pointsPerCircle, uvCordU));
				geometry.normals.push_back(normal);
			}
		}

		static void generateTestQuad(GEOMETRY& geometry) {
			geometry.vertices.push_back(Vector3f(-3, 3, 0));
			geometry.vertices.push_back(Vector3f(-3, -3, 0));
			geometry.vertices.push_back(Vector3f(3, -3, 0));
			geometry.vertices.push_back(Vector3f(3, 3, 0));
			geometry.uvs.push_back(Vector2f(0, 1));
			geometry.uvs.push_back(Vector2f(0, 0));
			geometry.uvs.push_back(Vector2f(1, 0));
			geometry.uvs.push_back(Vector2f(1, 1));
			geometry.normals.push_back(Vector3f(0, 0, 1));
			geometry.faces.push_back({ Vector3i(1, 1, 1), Vector3i(2, 2, 1), Vector3i(3, 3, 1), Vector3i(4, 4, 1) });
		}

		static float randomF(float min, float max) {
			return (min + (float)rand() / (float)(RAND_MAX) * (max - min));
		}

		static int randomI(int min, int max) {
			return min + (rand() % (max - min + 1));
		}
};
#endif