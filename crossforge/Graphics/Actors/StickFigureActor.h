/*****************************************************************************\
*                                                                           *
* File(s): StickFigureActor.h and StickFigureActor.cpp                                            *
*                                                                           *
* Content: Actor that visualizes the skeleton of a skeltal animation.   *
*                        *
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
#ifndef __CFORGE_STICKFIGUREACTOR_H__
#define __CFORGE_STICKFIGUREACTOR_H__

#include "SkeletalActor.h"
#include "StaticActor.h"
#include "../SceneGraph/SceneGraph.h"
#include "../SceneGraph/SGNGeometry.h"
#include "../SceneGraph/SGNTransformation.h"

namespace CForge {
	class CFORGE_API StickFigureActor : public SkeletalActor {
	public:
		StickFigureActor(void);
		~StickFigureActor(void);

		void init(T3DMesh<float>* pMesh, SkeletalAnimationController* pController);
		void clear(void);

		void render(class RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale);

		void jointMaterial(const T3DMesh<float>::Material Mat);
		void boneMaterial(const T3DMesh<float>::Material Mat);
		void jointSize(float Size);
		void boneSize(float Size);

		T3DMesh<float>::Material jointMaterial(void)const;
		T3DMesh<float>::Material boneMaterial(void)const;
		float jointSize(void)const;
		float boneSize(void)const;

		uint32_t jointCount(void)const;
		uint32_t boneCount(void)const;
		bool jointActive(uint32_t ID)const;
		bool boneActive(uint32_t ID)const;
		void jointActive(uint32_t ID, bool Active);
		void boneActive(uint32_t ID, bool Active);


	protected:
		void buildMaterial(T3DMesh<float>::Material* pMat);
		void createBone(T3DMesh<float>::Bone* pBone, SGNTransformation* pParent);

		StaticActor m_Joint;
		StaticActor m_Bone;

		SceneGraph m_SG;
		SGNTransformation m_RootSGN;
		std::vector<SGNGeometry*> m_JointSGNs;
		std::vector<SGNTransformation*> m_JointTransformSGNs;
		std::vector<SGNGeometry*> m_BoneSGNs;

		std::vector<SkeletalAnimationController::SkeletalJoint*> m_JointValues;

		float m_JointSize;
		float m_BoneSize;
	};//StickFigureActor

}//name space

#endif 