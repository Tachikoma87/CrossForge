#ifndef __CFORGE_ROCKMESH_H__
#define __CFORGE_ROCKMESH_H__

#include "../Core/CForgeObject.h"
#include "../Core/CoreUtility.hpp"
#include "T3DMesh.hpp"
#include "SAssetIO.h"
#include "PerlinNoise.hpp"
#include "CellularNoise3D.hpp"

using namespace Eigen;

namespace CForge {

	class RockMesh : public T3DMesh<float> {
		

		public:
			RockMesh() : T3DMesh() {
				// load base mesh (sphere)
				SAssetIO::load("Assets/baseRockShape.obj", this);

				// generate noise
				CellularNoise3D cNoise(10, 1, 1, 1);
				CellularNoise3D fineCNoise(50, 1, 1, 1);
				siv::PerlinNoise pNoise;

				// rough stone shape
				calcNormals();
				for (int i = 0; i < m_Positions.size(); i++) {	
					m_Positions[i] += m_Normals[i] * cNoise.getValue(m_Positions[i], 0.3, 3);
				}

				// fine cut
				calcNormals();
				for (int i = 0; i < m_Positions.size(); i++) {
					m_Positions[i] += m_Normals[i] * (pNoise.accumulatedOctaveNoise3D(m_Positions[i].x(), m_Positions[i].y(), m_Positions[i].z(), 8) / 7.5);
				}

				calcNormals();
			}

			protected:
				void calcNormals() {
					computePerVertexNormals(true);

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

					// average the normal within every group
					for (int i = 0; i < uniqueVertices.size(); i++) {
						for (int j = 1; j < uniqueVertices[i].size(); j++) {
							m_Normals[uniqueVertices[i][0]] += m_Normals[uniqueVertices[i][j]];
						}
						m_Normals[uniqueVertices[i][0]].normalize();
						for (int j = 1; j < uniqueVertices[i].size(); j++) {
							m_Normals[uniqueVertices[i][j]] = m_Normals[uniqueVertices[i][0]];
						}
					}

				}
	};
}

#endif