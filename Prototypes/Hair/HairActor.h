/*****************************************************************************\
*                                                                           *
* File(s): HairActor.hpp                                                    *
*                                                                           *
* Content:                                                                  *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann, Sascha Jüngel                                     *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_HAIRACTOR_H__
#define __CFORGE_HAIRACTOR_H__

#include "../../CForge/Graphics/Actors/IRenderableActor.h"
#include "../../CForge/Graphics/GLBuffer.h"
#include "../../CForge/Graphics/GLVertexArray.h"
#include "../../CForge/Graphics/Shader/GLShader.h"
#include "../../CForge/Graphics/RenderMaterial.h"
#include "../../CForge/Graphics/RenderDevice.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Core/SLogger.h"
#include "T3DHair.hpp"

namespace CForge {
	/**
	* \brief Actor that can be placed inside the world and does not do anything besides being rendered or moved rigidly.
	*
	* \todo Do full documentation
	*/
	class CFORGE_API HairActor: public IRenderableActor {
	public:
		HairActor(void);
		~HairActor(void);

		void init(const T3DHair<float>* pHair);
		void clear(void);
		void release(void);

		RenderMaterial* material();

		void render(RenderDevice* pRDev);

	protected:

		void initBufferData(void);

		void buildBuffer(void** ppBuffer, uint32_t* pBufferSize, const T3DHair<float>* pHair);

		void initShaders(const T3DHair<float>* pHair);

	private:
		RenderMaterial m_material; ///< Material data
		GLShader *m_pShader;
		uint32_t m_numStrands;
		int* m_start;
		int* m_count;

	};//HairActor

}//name space

#endif