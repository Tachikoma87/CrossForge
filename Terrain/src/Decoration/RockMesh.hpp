#ifndef __CFORGE_ROCKMESH_H__
#define __CFORGE_ROCKMESH_H__

#include "CForge/Core/CForgeObject.h"
#include "CForge/Core/CoreUtility.hpp"
#include "CForge/AssetIO/T3DMesh.hpp"
#include "CForge/AssetIO/SAssetIO.h"
#include "PerlinNoise.hpp"
#include "CellularNoise3D.hpp"

using namespace Eigen;

namespace CForge {

	class RockMesh : public T3DMesh<float> {
		

		public:
			RockMesh() : T3DMesh() {
				// load base mesh (sphere)
				//SAssetIO::load("Assets/uvSphere.obj", this);
				SAssetIO::load("Assets/uvSphereLowPoly.obj", this);
				//SAssetIO::load("Assets/plane.obj", this);
				//SAssetIO::load("Assets/cube.obj", this);
				
				
				// generate noise
				CellularNoise3D cNoise(10, 1, 1, 1);
				CellularNoise3D fineCNoise(50, 1, 1, 1);
				siv::PerlinNoise pNoise;

				// rough stone shape
				updateVertexAtributes();
				for (int i = 0; i < m_Positions.size(); i++) {
					m_Positions[i] += m_Normals[i] * cNoise.getValue(m_Positions[i], 0, 4);
				}

				// fine cut
				updateVertexAtributes();
				for (int i = 0; i < m_Positions.size(); i++) {
					m_Positions[i] += m_Normals[i] * (pNoise.accumulatedOctaveNoise3D(m_Positions[i].x(), m_Positions[i].y(), m_Positions[i].z(), 8) / 7.5);
				}
				
				// upscale
				/*
				float scale = 3;
				for (int i = 0; i < m_Positions.size(); i++) {
					m_Positions[i] *= scale;
				}
				*/

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