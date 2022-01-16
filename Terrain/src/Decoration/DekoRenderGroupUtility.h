#pragma once

#include "CForge/Graphics/Actors/RenderGroupUtility.h"
#include "CForge/Core/SLogger.h"
#include "CForge/Graphics/Shader/SShaderManager.h"

namespace CForge {
	class CFORGE_IXPORT DekoRenderGroupUtility : public RenderGroupUtility {
	public:
		void buildDekoIndexArray(const T3DMesh<float>* pMesh, void** ppBuffer, uint32_t* pBufferSize);
		void dekoInit(const T3DMesh<float>* pMesh, void** ppBuffer, uint32_t* pBufferSize);

	private:
		std::vector<RenderGroup*> m_RenderGroups;

	};
}