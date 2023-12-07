#pragma once

#include <CForge/Core/CoreUtility.hpp>
#include <iostream>
#include <fstream>
#include <string>

using namespace Eigen;
using namespace std;

struct GEOMETRY {
	std::vector<Vector3f> vertices;
	std::vector<Vector2f> uvs;
	std::vector<Vector3f> normals;
	std::vector<std::vector<Vector3i>> faces;
};

class ObjExporter {
	public:
		static void meshExport(GEOMETRY& geometry, string exportPath) {
			// open/create .obj file
			ofstream ObjFile(exportPath);
			string head = "# test Mesh\no test\n";

			// write and close file
			string content = geometryToString(geometry);
			cout << (head + content + "\n\n");
			ObjFile << (head + content);
			ObjFile.close();
		}

	protected:
		static string geometryToString(GEOMETRY& geometry) {
			string ret;
			for (auto& vertice : geometry.vertices) {
				ret += "v " + to_string(vertice.x()) + " " + to_string(vertice.y()) + " " + to_string(vertice.z()) + "\n";
			}
			for (auto& uv : geometry.uvs) {
				ret += "vt " + to_string(uv.x()) + " " + to_string(uv.y()) + "\n";
			}
			for (auto& normal : geometry.normals) {
				ret += "vn " + to_string(normal.x()) + " " + to_string(normal.y()) + " " + to_string(normal.z()) + "\n";
			}
			ret += "s off\n";
			for (auto& face : geometry.faces) {
				ret += "f";
				for (auto triplet : face) {
					ret += " " + to_string(triplet.x()) + "/" + to_string(triplet.y()) + "/" + to_string(triplet.z());
				}
				ret += "\n";
			}
			return ret;
		}
};
