/*****************************************************************************\
*                                                                           *
* File(s): StaticActor.h and StaticActor.cpp                                *
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
#ifndef __CFORGE_STATICACTOR_H__
#define __CFORGE_STATICACTOR_H__

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
	class CFORGE_IXPORT StaticActor: public IRenderableActor {
	public:
		StaticActor(void);
		~StaticActor(void);

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

	};//StaticActor

}//name space


#endif