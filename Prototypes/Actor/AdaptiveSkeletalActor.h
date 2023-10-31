/*****************************************************************************\
*                                                                           *
* File(s): AdaptiveSkeletalActor.h and AdaptiveSkeletalActor.cpp                               *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_ADAPTIVESKELETALACTOR_H__
#define __CFORGE_ADAPTIVESKELETALACTOR_H__

#include <crossforge/Graphics/Actors/SkeletalActor.h>
#include "../Animation/SkeletalSkinning.h"

namespace CForge {
	class AdaptiveSkeletalActor : public SkeletalActor {
	public:
		AdaptiveSkeletalActor(void);
		~AdaptiveSkeletalActor(void);

		void init(T3DMesh<float>* pMesh, SkeletalAnimationController* pController) override;
		void render(RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) override;
	protected:

		SkeletalSkinning m_SkeletalSkinning;
		std::vector<SkeletalAnimationController::SkeletalJoint*> m_Joints;

	};//AdaptiveSkeletalActor
}

#endif 