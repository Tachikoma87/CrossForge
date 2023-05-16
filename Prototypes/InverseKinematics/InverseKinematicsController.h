#ifndef __CFORGE_INVERSEKINEMATICSCONTROLLER_H__
#define __CFORGE_INVERSEKINEMATICSCONTROLLER_H__

#include "../../crossforge/AssetIO/T3DMesh.hpp"
#include "../../crossforge/Graphics/UniformBufferObjects/UBOBoneData.h"
#include "../../crossforge/Graphics/Shader/ShaderCode.h"
#include "../../crossforge/Graphics/Shader/GLShader.h"

#include <nlohmann/json.hpp>

#include <map>

namespace CForge {
	class InverseKinematicsController: public CForgeObject {
	public:
		enum SkeletalSegment {
			NONE = -1,
			RIGHT_ARM = 0,
			LEFT_ARM = 1,
			RIGHT_LEG = 2,
			LEFT_LEG = 3,
			SPINE = 4,
			HEAD = 5
		};

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
			SkeletalSegment Segment;
			Eigen::Vector3f Target;

			SkeletalEndEffector(void) : CForgeObject("InverseKinematicsController::SkeletalEndEffector") {
				JointID = -1;
				Segment = NONE;
			}
		};

		InverseKinematicsController(void);
		~InverseKinematicsController(void);

		// pMesh has to hold skeletal definition
		void init(T3DMesh<float>* pMesh, std::string ConfigFilepath, Eigen::Vector3f GlobalActorPosition, Eigen::Quaternionf GlobalActorRotation, Eigen::Vector3f GlobalActorScaling);
		void update(float FPSScale);
		void clear(void);

		void applyAnimation(bool UpdateUBO = true);

		UBOBoneData* ubo(void);

		GLShader* shadowPassShader(void);

		void retrieveSkinningMatrices(std::vector<Eigen::Matrix4f>* pSkinningMats);

		std::vector<SkeletalJoint*> retrieveSkeleton(void) const;
		void updateSkeletonValues(std::vector<SkeletalJoint*>* pSkeleton);

		std::vector<SkeletalEndEffector*> retrieveEndEffectors(void) const;
		void updateEndEffectorValues(std::vector<SkeletalEndEffector*>* pEndEffectors);
		void endEffectorTarget(SkeletalSegment SegmentID, Eigen::Vector3f NewTarget);

		void globalActorPosition(Eigen::Vector3f Position);
		void globalActorRotation(Eigen::Quaternionf Rotation);
		void globalActorScaling(Eigen::Vector3f Scaling);
		Eigen::Vector3f globalActorPosition(void) const;
		Eigen::Quaternionf globalActorRotation(void) const;
		Eigen::Vector3f globalActorScaling(void) const;

	protected:
		enum ConstraintType {
			UNCONSTRAINED = 0,
			HINGE = 1,
			BALL_AND_SOCKET = 2
			// ...
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
			ConstraintType ConstraintType;
			//TODO: constraint data...
		};
		
		struct HeadJoint {
			Joint* pJoint;
			Eigen::Vector3f CurrentGlobalDir;
			Eigen::Vector3f Target;
		};

		struct KinematicChain {
			std::vector<Joint*> Joints; // Joints.begin() == end-effector; Joints.back() == root of chain (not necessarily root of skeleton)
			Eigen::Vector3f Target;
		};

		// end-effector -> root CCD IK
		void ikCCD(SkeletalSegment SegmentID);
		void rotateGaze(void);
		void constrainLocalRotation(Joint* pJoint);

		void forwardKinematics(Joint* pJoint, Eigen::Vector3f ParentPosition, Eigen::Quaternionf ParentRotation);
		void updateSkinningMatrices(Joint* pJoint, Eigen::Matrix4f ParentTransform);
		int32_t jointIDFromName(std::string JointName);
		
		void initJointConstraints(const nlohmann::json& ConstraintDefinitions);
		void buildKinematicChain(SkeletalSegment SegmentID, const nlohmann::json& ChainData);
		
		Eigen::Vector3f m_GlobalActorPosition;
		Eigen::Quaternionf m_GlobalActorRotation;
		Eigen::Vector3f m_GlobalActorScaling;

		Joint* m_pRoot;
		std::vector<Joint*> m_Joints;
		HeadJoint* m_pHead;
		std::map<SkeletalSegment, KinematicChain> m_KinematicChains;

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