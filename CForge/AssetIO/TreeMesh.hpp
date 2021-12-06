#ifndef __CFORGE_TREEMESH_H__
#define __CFORGE_TREEMESH_H__

#include "../Core/CForgeObject.h"
#include "../Core/CoreUtility.hpp"
#include "T3DMesh.hpp"
#include "SAssetIO.h"

using namespace Eigen;

namespace CForge {

	class TreeMesh : public T3DMesh<float> {
		

		public:
			TreeMesh() : T3DMesh() {
				SAssetIO::load("Assets/tree0.obj", this);
				//computePerFaceNormals();
				//calcTangents();
				updateVertexAtributes();
				//printf("Amount of Vertices: %d\n", m_Positions.size());
			}

			protected:
				void updateVertexAtributes() {
					computePerFaceNormals();
					calcTangents();

					std::vector<std::vector<int>> uniqueVertices;
					uniqueVertices.push_back(std::vector<int>());
					uniqueVertices[0].push_back(0);

					// group vertices with same position
					for (int i = 1; i < m_Positions.size(); i++) {
						bool isUnique = true;
						for (int j = 0; j < uniqueVertices.size(); j++) {
							if ((m_Positions[i] - m_Positions[uniqueVertices[j][0]]).norm() < 0.001) {
								isUnique = false;
								uniqueVertices[j].push_back(i);
							}
						}
						if (isUnique) {
							uniqueVertices.push_back(std::vector<int>());
							uniqueVertices[uniqueVertices.size() - 1].push_back(i);
						}
					}

					// average the normal/tangent within every group
					for (int i = 0; i < uniqueVertices.size(); i++) {
						for (int j = 1; j < uniqueVertices[i].size(); j++) {
							m_Normals[uniqueVertices[i][0]] += m_Normals[uniqueVertices[i][j]];
							m_Tangents[uniqueVertices[i][0]] += m_Tangents[uniqueVertices[i][j]];
						}
						m_Normals[uniqueVertices[i][0]].normalize();
						m_Tangents[uniqueVertices[i][0]].normalize();
						for (int j = 1; j < uniqueVertices[i].size(); j++) {
							m_Normals[uniqueVertices[i][j]] = m_Normals[uniqueVertices[i][0]];
							m_Tangents[uniqueVertices[i][j]] = m_Tangents[uniqueVertices[i][0]];
						}
					}


				}

				void calcTangents() {
					m_Tangents.clear();
					for (int i = 0; i < m_Positions.size(); i++) {
						m_Tangents.push_back(Vector3f(1, 0, 1));
					}
					for (auto i : m_Submeshes) {
						for (auto face : i->Faces) {
							Vector3f deltaPos1 = m_Positions[face.Vertices[1]] - m_Positions[face.Vertices[0]];
							Vector3f deltaPos2 = m_Positions[face.Vertices[2]] - m_Positions[face.Vertices[0]];
							Vector3f deltaUV1 = m_UVWs[face.Vertices[1]] - m_UVWs[face.Vertices[0]];
							Vector3f deltaUV2 = m_UVWs[face.Vertices[2]] - m_UVWs[face.Vertices[0]];
							float r = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
							
							for (int j = 0; j < 3; j++) {
								m_Tangents[face.Vertices[j]] = Vector3f((deltaPos1 * deltaUV2.y() - deltaPos2 * deltaUV1.y()) * r);
							}
						}
					}
				}
	};
}
#endif