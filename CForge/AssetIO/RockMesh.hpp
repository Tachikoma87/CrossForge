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
				SAssetIO::load("Assets/rockSphereBase.obj", this);
				CellularNoise3D cNoise(0.5f, 2, 2, 2);
				siv::PerlinNoise pNoise;
				for (int i = 0; i < m_Positions.size(); i++) {
					Vector3f pos = m_Positions[i];
					
					m_Positions[i] *= cNoise.getValue(m_Positions[i], 2, 2.6);
					m_Positions[i] *= pNoise.accumulatedOctaveNoise3D(pos.x(), pos.y(), pos.z(), 8) / 1.0f + 1;
				}
			}
	};
}

#endif