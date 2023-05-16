#ifndef __CFORGE_IKSTICKFIGUREACTOR_H__
#define __CFORGE_IKSTICKFIGUREACTOR_H__

#include "IKSkeletalActor.h"
#include "../../crossforge/Graphics/Actors/StaticActor.h"
#include "../../crossforge/Graphics/SceneGraph/SceneGraph.h"
#include "../../crossforge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../crossforge/Graphics/SceneGraph/SGNTransformation.h"

namespace CForge {
	class IKStickFigureActor : public IKSkeletalActor {
	public:
		IKStickFigureActor(void);
		~IKStickFigureActor(void);

		void init(T3DMesh<float>* pMesh, InverseKinematicsController* pController);
		void clear(void);
		void release(void);

		void render(class RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale);

		void jointMaterial(const T3DMesh<float>::Material Mat);
		void boneMaterial(const T3DMesh<float>::Material Mat);
		void jointSize(float Size);
		void boneSize(float Size);
		//void jointAxesSize(float Size);

		T3DMesh<float>::Material jointMaterial(void)const;
		T3DMesh<float>::Material boneMaterial(void)const;
		float jointSize(void)const;
		float boneSize(void)const;


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
		
		std::vector<InverseKinematicsController::SkeletalJoint*> m_JointValues;

		float m_JointSize;
		float m_BoneSize;
	};//IKStickFigureActor

}//name space

#endif 