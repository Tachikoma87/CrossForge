#pragma once

#include "IRenderableActor.h"
#include "../../AssetIO/T3DMesh.hpp"
#include "../GLBuffer.h"
#include "../GLVertexArray.h"
#include "../Shader/GLShader.h"
#include "../RenderMaterial.h"
#include "VertexUtility.h"
#include "RenderGroupUtility.h"

namespace CForge {
	/**
	* \brief Actor that can be placed inside the world and does not do anything besides being rendered or moved rigidly.
	*
	* \todo Do full documentation
	*/
	class CFORGE_IXPORT InstanceActor: public IRenderableActor {
	public:
		InstanceActor(void);
		~InstanceActor(void);

		void init(const T3DMesh<float>* pMesh);
		void clear(void);
		void release(void);

		void render(RenderDevice* pRDev);

		uint32_t materialCount(void)const;
		RenderMaterial* material(uint32_t Index);

	protected:

	private:
		//void setBufferData(void);
		/*
		GLBuffer m_ArrayBuffer; ///< stores vertex data
		GLBuffer m_ElementBuffer; ///< stores triangle indexes
		GLVertexArray m_VertexArray; ///< the vertex array

		VertexUtility m_VertexUtility;
		RenderGroupUtility m_RenderGroupUtility;*/

	};

}
