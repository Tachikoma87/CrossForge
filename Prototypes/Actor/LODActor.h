/*****************************************************************************\
*                                                                           *
* File(s): LODActor.h and LODActor.cpp                                *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_AABBACTOR_H__
#define __CFORGE_AABBACTOR_H__

#include "../../CForge/Graphics/Actors/IRenderableActor.h"
#include "../../CForge/AssetIO/T3DMesh.hpp"
#include "../../CForge/Graphics/GLBuffer.h"
#include "../../CForge/Graphics/GLVertexArray.h"
#include "../../CForge/Graphics/Shader/GLShader.h"
#include "../../CForge/Graphics/RenderMaterial.h"
#include "../../CForge/Graphics/Actors/VertexUtility.h"
#include "../../CForge/Graphics/Actors/RenderGroupUtility.h"
#include "../SLOD.h"

namespace CForge {
	/**
	* \brief Actor that can be placed inside the world and does not do anything besides being rendered or moved rigidly.
	*
	* \todo Do full documentation
	*/
	class LODActor : public IRenderableActor {
	public:
		LODActor(void);
		~LODActor(void);

		void init(T3DMesh<float>* pMesh);
		void init(T3DMesh<float>* pMesh, bool isTranslucent);
		void init(T3DMesh<float>* pMesh, bool isTranslucent, const std::vector<float> &LODStages);
		void clear(void);
		void release(void);

		void render(RenderDevice* pRDev);

		uint32_t materialCount(void)const;
		RenderMaterial* material(uint32_t level);
		
		void bindLODLevel(uint32_t index);
		bool isTranslucent();
		
	protected:

	private:
		SLOD* m_pSLOD;
		
		std::vector<T3DMesh<float>*> m_LODMeshes;
		std::vector<float> m_LODStages;
		uint32_t m_LODLevel;
		
		bool m_translucent;
		bool m_visible;
		uint32_t m_pixelCount;
		
		//void setBufferData(void);
		/*
		GLBuffer m_ArrayBuffer; ///< stores vertex data
		GLBuffer m_ElementBuffer; ///< stores triangle indexes
		GLVertexArray m_VertexArray; ///< the vertex array

		VertexUtility m_VertexUtility;
		RenderGroupUtility m_RenderGroupUtility;*/

	};//LODActor

}//name space


#endif