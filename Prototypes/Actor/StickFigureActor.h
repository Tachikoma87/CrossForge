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
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_STICKFIGUREACTOR_H__
#define __CFORGE_STICKFIGUREACTOR_H__

#include <CForge/Graphics/Actors/SkeletalActor.h>
#include <CForge/Graphics/Actors/StaticActor.h>
#include <CForge/Graphics/SceneGraph/SceneGraph.h>
#include <CForge/Graphics/SceneGraph/SGNGeometry.h>
#include <CForge/Graphics/SceneGraph/SGNTransformation.h>

namespace CForge {
	class StickFigureActor : public SkeletalActor {
	public:
		StickFigureActor(void);
		~StickFigureActor(void);

		void init(T3DMesh<float>* pMesh, SkeletalAnimationController *pController);
		void clear(void);
		void release(void);

		void jointColor(const Eigen::Vector4f Col);
		void boneColor(const Eigen::Vector4f Col);

		void jointSize(float Size);
		void boneSize(float Size);


		void render(class RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale);
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



	};//StickFigureActor

}//name space

#endif 