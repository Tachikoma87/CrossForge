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
			Eigen::Matrix3Xf EndEffectorPoints;
			Eigen::Matrix3Xf TargetPoints;

			SkeletalEndEffector(void) : CForgeObject("InverseKinematicsController::SkeletalEndEffector") {
				JointID = -1;
				Segment = NONE;
			}
		};

		InverseKinematicsController(void);
		~InverseKinematicsController(void);

		// pMesh has to hold skeletal definition
		void init(T3DMesh<float>* pMesh, std::string ConfigFilepath);
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
		void translateTarget(SkeletalSegment SegmentID, Eigen::Vector3f Translation);

		Eigen::Vector3f rootPosition(void); //TODO
		void rootPosition(Eigen::Vector3f Position); //TODO

	protected:
		enum ConstraintType {
			UNCONSTRAINED = 0,
			HINGE = 1,
			BALL_AND_SOCKET = 2
			// ...
		};

		struct EndEffectorData {
			Eigen::Matrix3Xf LocalEndEffectorPoints;
			Eigen::Matrix3Xf GlobalEndEffectorPoints;
			Eigen::Matrix3Xf GlobalTargetPoints;
			Eigen::Quaternionf GlobalTargetRotation;
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
			ConstraintType ConstraintType;
			Eigen::Vector3f LocalHingeAxis;
			
			//TODO: constraint data...
		};
		
		struct HeadJoint {
			Joint* pJoint;
			Eigen::Vector3f CurrentGlobalDir;
			Eigen::Vector3f Target;
		};

		void initJointProperties(T3DMesh<float>* pMesh, const nlohmann::json& ConstraintData);
		void initSkeletonStructure(T3DMesh<float>* pMesh, const nlohmann::json& StructureData);
		void buildKinematicChain(SkeletalSegment SegmentID, const nlohmann::json& ChainData);
		void initEndEffectorPoints(const nlohmann::json& EndEffectorPropertiesData);
		void initTargetPoints(void);

		// end-effector -> root CCD IK
		void ikCCD(SkeletalSegment SegmentID);
		void rotateGaze(void);
		Eigen::Quaternionf computeUnconstrainedGlobalRotation(Joint* pJoint, InverseKinematicsController::EndEffectorData* pEffData);
		void constrainLocalRotation(Joint* pJoint);
		void forwardKinematics(Joint* pJoint);

		void updateSkinningMatrices(Joint* pJoint, Eigen::Matrix4f ParentTransform);
		int32_t jointIDFromName(std::string JointName);
		
		Joint* m_pRoot;
		std::vector<Joint*> m_Joints;
		HeadJoint* m_pHead;
		std::map<SkeletalSegment, std::vector<Joint*>> m_JointChains; // Joints.front() is end-effector joint

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