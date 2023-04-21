#ifndef __CFORGE_INVERSEKINEMATICSCONTROLLER_H__
#define __CFORGE_INVERSEKINEMATICSCONTROLLER_H__

#include "../../crossforge/AssetIO/T3DMesh.hpp"
#include "../../crossforge/Graphics/UniformBufferObjects/UBOBoneData.h"
#include "../../crossforge/Graphics/Shader/ShaderCode.h"
#include "../../crossforge/Graphics/Shader/GLShader.h"

namespace CForge {
	class InverseKinematicsController: public CForgeObject {
	public:
		struct SkeletalJoint : public CForgeObject {
			int32_t ID;
			std::string Name;
			Eigen::Matrix4f OffsetMatrix;
			Eigen::Vector3f LocalPosition;
			Eigen::Quaternionf LocalRotation;
			Eigen::Vector3f LocalScale;
			Eigen::Matrix4f SkinningMatrix;
			
			int32_t Parent;
			std::vector<int32_t> Children;

			SkeletalJoint(void) : CForgeObject("InverseKinematicsController::SkeletalJoint") {
				ID = -1;
				Parent = -1;
			}

		};

		struct SkeletalEndEffector : public CForgeObject {
			int32_t JointID;
			std::string JointName;
			Eigen::Vector3f Target;

			SkeletalEndEffector(void) : CForgeObject("InverseKinematicsController::SkeletalEndEffector") {
				JointID = -1;
			}
		};

		InverseKinematicsController(void);
		~InverseKinematicsController(void);

		// pMesh has to hold skeletal definition
		void init(T3DMesh<float>* pMesh, Eigen::Vector3f GlobalActorPosition, Eigen::Quaternionf GlobalActorRotation, Eigen::Vector3f GlobalActorScaling);
		void update(float FPSScale);
		void clear(void);

		void applyAnimation(bool UpdateUBO = true);

		UBOBoneData* ubo(void);

		GLShader* shadowPassShader(void);

		void retrieveSkinningMatrices(std::vector<Eigen::Matrix4f>* pSkinningMats);

		std::vector<SkeletalJoint*> retrieveSkeleton(void) const;
		void updateSkeletonValues(std::vector<SkeletalJoint*>* pSkeleton);

		void globalActorPosition(Eigen::Vector3f Position);
		void globalActorRotation(Eigen::Quaternionf Rotation);
		void globalActorScaling(Eigen::Vector3f Scaling);
		Eigen::Vector3f globalActorPosition(void) const;
		Eigen::Quaternionf globalActorRotation(void) const;
		Eigen::Vector3f globalActorScaling(void) const;

		std::vector<SkeletalEndEffector*> retrieveEndEffectors(void) const;
		void endEffectorTarget(const SkeletalEndEffector* pModifiedEndEffector);

	protected:		
		struct EndEffectorData {
			Eigen::Vector3f Target;
		};

		struct Joint {
			int32_t ID;
			std::string Name;
			Eigen::Matrix4f OffsetMatrix;
			Eigen::Vector3f LocalPosition;
			Eigen::Quaternionf LocalRotation;
			Eigen::Vector3f LocalScale;
			Eigen::Vector3f GlobalPosition;
			Eigen::Quaternionf GlobalRotation;
			Eigen::Matrix4f SkinningMatrix;
			
			Joint* pParent;
			std::vector<Joint*> Children;
			EndEffectorData* pEndEffectorData;
		};
		
		// end-effector -> root CCDIK
		void TopDownCCDIK(Joint* pEndEffector, std::vector<Joint*>* Chain);

		void forwardKinematics(Joint* pJoint, Eigen::Vector3f ParentPosition, Eigen::Quaternionf ParentRotation);
		void updateSkinningMatrices(Joint* pJoint, Eigen::Matrix4f ParentTransform);
		int32_t jointIDFromName(std::string JointName);
		std::vector<Joint*> buildKinematicChainToRoot(Joint* pEndEffector);

		Eigen::Vector3f m_GlobalActorPosition;
		Eigen::Quaternionf m_GlobalActorRotation;
		Eigen::Vector3f m_GlobalActorScaling;

		Joint* m_pRoot;
		std::vector<Joint*> m_Joints;

		int32_t m_MaxIterations;
							
		UBOBoneData m_UBO;
		GLShader *m_pShadowPassShader;
		ShaderCode* m_pShadowPassVSCode;
		ShaderCode* m_pShadowPassFSCode;

		std::string m_GLSLVersionTag;
		std::string m_GLSLPrecisionTag;
	};//InverseKinematicsController

}//name space

#endif 