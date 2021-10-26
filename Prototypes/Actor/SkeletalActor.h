/*****************************************************************************\
*                                                                           *
* File(s): SkeletalActor.h and SkeletalActor.cpp                           *
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
#ifndef __CFORGE_SKELETALACTOR_H__
#define __CFORGE_SKELETALACTOR_H__

#include "../../CForge/Core/CForgeObject.h"
#include "../../CForge/Graphics/Actors/IRenderableActor.h"
#include "../../CForge/Graphics/Actors/VertexUtility.h"
#include "../../CForge/Graphics/Actors/RenderGroupUtility.h"
#include "SkeletalAnimationController.h"

namespace CForge {
	class SkeletalActor : public IRenderableActor {
	public:
		SkeletalActor(void);
		~SkeletalActor(void);

		void init(T3DMesh<float>* pMesh, SkeletalAnimationController* pController);
		void activeAnimation(SkeletalAnimationController::Animation* pAnim);
		SkeletalAnimationController::Animation* activeAnimation(void)const;
		void clear(void);
		void release(void);

		void render(class RenderDevice* pRDev);

	protected:
		SkeletalAnimationController* m_pAnimationController;
		SkeletalAnimationController::Animation* m_pActiveAnimation;

	};//SkeletalActor

}//name-space

#endif 