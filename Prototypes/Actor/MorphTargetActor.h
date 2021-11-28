/*****************************************************************************\
*                                                                           *
* File(s): MorphTargetActor.h and MorphTargetActor.cpp                          *
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
#ifndef __CFORGE_MORPHTARGETACTOR_H__
#define __CFROGE_MORPHTARGETACTOR_H__

#include "../../CForge/Graphics/Actors/IRenderableActor.h"
#include "MorphTargetAnimationController.h"
#include "../../CForge//Graphics/GLBuffer.h"
#include "UBOMorphTargetData.h"

namespace CForge {
	class MorphTargetActor : public IRenderableActor {
	public:
		MorphTargetActor(void);
		~MorphTargetActor(void);

		void init(T3DMesh<float>* pMesh);
		void clear(void);
		void release(void);

		void render(class RenderDevice* pRDev);
		void update(float fpsScale = 1.0f);

	protected:
		void buildMorphTargetBuffer(T3DMesh<float>* pMesh);

		GLBuffer m_MorphTargetBuffer;
		UBOMorphTargetData m_MorphTargetUBO;
	};//MorphTargetActor

}//name-space

#endif 