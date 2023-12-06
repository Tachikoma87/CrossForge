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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SKELETALACTOR_H__
#define __CFORGE_SKELETALACTOR_H__

#include "IRenderableActor.h"
#include "../Controller/SkeletalAnimationController.h"

namespace CForge {
	class CFORGE_API SkeletalActor : public IRenderableActor {
	public:
		SkeletalActor(void);
		~SkeletalActor(void);

		virtual void init(T3DMesh<float>* pMesh, SkeletalAnimationController* pController, bool PrepareCPUSkinning = false);
		virtual void activeAnimation(SkeletalAnimationController::Animation* pAnim);
		virtual SkeletalAnimationController::Animation* activeAnimation(void)const;
		virtual void clear(void);
		virtual void release(void);

		virtual void render(class RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale);

		virtual Eigen::Vector3f transformVertex(int32_t Index);

	protected:
		virtual void prepareCPUSkinning(const T3DMesh<float>* pMesh);

		/**
		* \brief Structure that holds data for CPU skinning.
		*/
		struct SkinVertex {
			Eigen::Vector3f V;
			std::vector<int32_t> BoneInfluences;
			std::vector<float> BoneWeights;
		};//SkinVertex

		SkeletalAnimationController* m_pAnimationController;
		SkeletalAnimationController::Animation* m_pActiveAnimation;
		std::vector<SkinVertex*> m_SkinVertexes;

	};//SkeletalActor

}//name-space

#endif 