#ifndef __CFORGE_IKSKELETALACTOR_H__
#define __CFORGE_IKSKELETALACTOR_H__

#include "../../crossforge/Graphics/Actors/IRenderableActor.h"
#include "InverseKinematicsController.h"

namespace CForge {
	class IKSkeletalActor : public IRenderableActor {
	public:
		IKSkeletalActor(void);
		~IKSkeletalActor(void);

		void init(T3DMesh<float>* pMesh, InverseKinematicsController* pController);
		void clear(void);
		void release(void);

		void render(class RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale);

	protected:
		InverseKinematicsController* m_pAnimationController;

	};//SkeletalActor

}//name-space

#endif 