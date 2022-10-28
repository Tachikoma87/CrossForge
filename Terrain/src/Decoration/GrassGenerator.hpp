#pragma once

#include "CForge/Core/CoreUtility.hpp"
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

class GrassGenerator {
	public:
		static void generateGrass(GrassType grassType, int amount, string exportPath) {
			for (int i = 0; i < amount; i++) {
				// new random Seed
				srand(time(NULL));
				
				// grass generation 
				GEOMETRY geometry;


				switch (grassType) {
					case cross:
						generate(geometry, 0, 6, 6);
						break;
					case triangle:
						generate(geometry, 0.6, 6, 3);
						break;
				}

				ObjExporter::meshExport(geometry, exportPath + to_string(i) + ".obj");
			}
		}

	protected:

		// generate grass
		static void generate(GEOMETRY& geometry, float radius, float size, int numberFaces) {
			Vector3f bitangent = Vector3f(0, 1, 0);
			Vector3f normal = Vector3f(0, 0, 1);
			Vector3f tangent = bitangent.cross(normal);
			Matrix3f tangentSpace;
		
			tangentSpace.col(1) = Vector3f(0, 1, 0);
	
			
			geometry.uvs.push_back(Vector2f(0, 1));
			geometry.uvs.push_back(Vector2f(0, 0));
			geometry.uvs.push_back(Vector2f(1, 0));
			geometry.uvs.push_back(Vector2f(1, 1));
			for (int i = 0; i < numberFaces; i++) {
				float angle = i * 2 * PI / numberFaces;
				tangentSpace.col(0) = Vector3f(cos(angle), 0, sin(angle));
				tangentSpace.col(2) = tangentSpace.col(1).cross(tangentSpace.col(0));

				float width = size / 2;// randomF(size / 4, size / 2);
				geometry.vertices.push_back(tangentSpace * Vector3f(-width, randomF(size / 2, size), radius));
				geometry.vertices.push_back(tangentSpace * Vector3f(-width, 0, radius));
				geometry.vertices.push_back(tangentSpace * Vector3f(width, 0, radius));
				geometry.vertices.push_back(tangentSpace * Vector3f(width, randomF(size / 2, size), radius));
				geometry.normals.push_back(tangentSpace.col(0));
				int lastIntex = geometry.vertices.size();
				geometry.faces.push_back({ Vector3i(lastIntex - 3, 1, geometry.normals.size()), Vector3i(lastIntex - 2, 2, geometry.normals.size()), Vector3i(lastIntex - 1, 3, geometry.normals.size()), Vector3i(lastIntex, 4, geometry.normals.size()) });
			}
		}

		static float randomF(float min, float max) {
			return (min + (float)rand() / (float)(RAND_MAX) * (max - min));
		}
};
