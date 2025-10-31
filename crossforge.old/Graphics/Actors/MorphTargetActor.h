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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_MORPHTARGETACTOR_H__
#define __CFORGE_MORPHTARGETACTOR_H__

#include "IRenderableActor.h"
#include "../GLBuffer.h"
#include "../UniformBufferObjects/UBOMorphTargetData.h"
#include "../Controller/MorphTargetAnimationController.h"

namespace CForge {
	class CFORGE_API MorphTargetActor : public IRenderableActor {
	public:
		MorphTargetActor(void);
		~MorphTargetActor(void);

		void init(T3DMesh<float>* pMesh, MorphTargetAnimationController *pController);
		void clear(void);

		void render(class RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale);
		void update(float fpsScale = 1.0f);

		int32_t addAnimation(MorphTargetAnimationController::ActiveAnimation* pAnim);

	protected:
		void buildMorphTargetBuffer(T3DMesh<float>* pMesh);

		uint32_t m_MorphTargetTexture;

		UBOMorphTargetData m_MorphTargetUBO;
		MorphTargetAnimationController* m_pAnimationController;

		std::vector<MorphTargetAnimationController::ActiveAnimation*> m_ActiveAnimations;
	};//MorphTargetActor

}//name-space

#endif 