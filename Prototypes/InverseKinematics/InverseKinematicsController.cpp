#include "InverseKinematicsController.h"
#include <crossforge/Graphics/Shader/SShaderManager.h>
#include <crossforge/Math/CForgeMath.h>

using namespace Eigen;
using namespace std;

namespace CForge {

	InverseKinematicsController::InverseKinematicsController(void): CForgeObject("InverseKinematicsController") {
		m_pRoot = nullptr;

		m_MaxIterations = 100;
		m_GlobalActorPosition = Vector3f::Zero();
		m_GlobalActorRotation = Quaternionf::Identity();
		m_GlobalActorScaling = Vector3f::Ones();

		m_pShadowPassShader = nullptr;
		m_pShadowPassFSCode = nullptr;
		m_pShadowPassVSCode = nullptr;

#ifdef SHADER_GLES
		m_GLSLVersionTag = "300 es";
		m_GLSLPrecisionTag = "lowp";
#else
		m_GLSLVersionTag = "330 core";
		m_GLSLPrecisionTag = "lowp";
#endif

	}//constructor

	InverseKinematicsController::~InverseKinematicsController(void) {
		clear();
	}//Destructor

	// pMesh has to hold skeletal definition
	void InverseKinematicsController::init(T3DMesh<float>* pMesh, Vector3f GlobalActorPosition, Quaternionf GlobalActorRotation, Vector3f GlobalActorScaling) {
		clear();

		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->boneCount() == 0) throw CForgeExcept("Mesh has no bones!");
		if (pMesh->skeletalAnimationCount() == 0) throw CForgeExcept("Mesh has no animation data!"); // use first keyframe of first animation as rest pose of skeleton for now

		// create bones and copy data
		T3DMesh<float>::SkeletalAnimation* pAnimation = pMesh->getSkeletalAnimation(0);

		for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
			const T3DMesh<float>::Bone* pRef = pMesh->getBone(i);

			Joint* pJoint = new Joint();
			pJoint->ID = pRef->ID;
			pJoint->Name = pRef->Name;
			pJoint->LocalPosition = pAnimation->Keyframes[i]->Positions[0];
			pJoint->LocalRotation = pAnimation->Keyframes[i]->Rotations[0];
			pJoint->LocalScale = pAnimation->Keyframes[i]->Scalings[0];
			pJoint->GlobalPosition = Vector3f::Zero();						// computed after joint hierarchy has been constructed
			pJoint->GlobalRotation = Quaternionf::Identity();				// computed after joint hierarchy has been constructed
			pJoint->OffsetMatrix = pRef->OffsetMatrix;
			pJoint->SkinningMatrix = Matrix4f::Identity();					// computed during applyAnimation()
			
			m_Joints.push_back(pJoint);	
		}//for[bones]

		// copy structure
		for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
			const T3DMesh<float>::Bone* pRef = pMesh->getBone(i);
			Joint* pJoint = m_Joints[i];

			if (pRef->pParent != nullptr) pJoint->pParent = m_Joints[pRef->pParent->ID];
			else pJoint->pParent = nullptr;

			for (uint32_t k = 0; k < pRef->Children.size(); ++k) pJoint->Children.push_back(m_Joints[pRef->Children[k]->ID]);
		}

		// find root bone
		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			if (m_Joints[i]->pParent == nullptr) {
				m_pRoot = m_Joints[i];
				break;
			}
		}//for[all bones]
		
		// initialize global positions and rotations of all joints given the world space transformation of the actor
		m_GlobalActorPosition = GlobalActorPosition;
		m_GlobalActorRotation = GlobalActorRotation;
		m_GlobalActorScaling = GlobalActorScaling;
		forwardKinematics(m_pRoot, GlobalActorPosition, GlobalActorRotation);

		// initialize end-effector data
		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			Joint* pJoint = m_Joints[i];

			if (pJoint->Children.empty()) { // leaf joints are used as end-effectors
				EndEffectorData* pData = new EndEffectorData();
				
				pData->Target = pJoint->GlobalPosition;

				pJoint->pEndEffectorData = pData;
			}
		}//for[joints]
				
		// initialize UBO
		m_UBO.init(m_Joints.size());

		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			m_UBO.skinningMatrix(i, Eigen::Matrix4f::Identity());
		}//for[bones]

		SShaderManager* pSMan = SShaderManager::instance();

		m_pShadowPassFSCode = pSMan->createShaderCode("Shader/ShadowPassShader.frag", m_GLSLVersionTag, 0, m_GLSLPrecisionTag);
		m_pShadowPassVSCode = pSMan->createShaderCode("Shader/ShadowPassShader.vert", m_GLSLVersionTag, ShaderCode::CONF_SKELETALANIMATION | ShaderCode::CONF_LIGHTING, m_GLSLPrecisionTag);

		ShaderCode::SkeletalAnimationConfig SkelConfig;
		SkelConfig.BoneCount = m_Joints.size();
		m_pShadowPassVSCode->config(&SkelConfig);

		std::vector<ShaderCode*> VSSources;
		std::vector<ShaderCode*> FSSources;
		VSSources.push_back(m_pShadowPassVSCode);
		FSSources.push_back(m_pShadowPassFSCode);

		m_pShadowPassShader = pSMan->buildShader(&VSSources, &FSSources, nullptr);

		pSMan->release();

	}//initialize

	void InverseKinematicsController::clear(void) {
		m_pRoot = nullptr;
		for (auto& i : m_Joints) if (nullptr != i) delete i;
		m_Joints.clear();

		m_UBO.clear();

		// instances get deleted by the Shader Manager
		m_pShadowPassShader = nullptr;
		m_pShadowPassFSCode = nullptr;
		m_pShadowPassVSCode = nullptr;

	}//clear

	GLShader* InverseKinematicsController::shadowPassShader(void) {
		return m_pShadowPassShader;
	}//shadowPassShader

	int32_t InverseKinematicsController::jointIDFromName(std::string JointName) {
		int32_t Rval = -1;

		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			if (m_Joints[i]->Name.compare(JointName) == 0) Rval = i;
		}

		return Rval;
	}//jointIDFromName

	std::vector<InverseKinematicsController::Joint*> InverseKinematicsController::buildKinematicChainToRoot(Joint* pEndEffector) {
		std::vector<Joint*> Rval;
		
		Joint* pJoint = pEndEffector->pParent;

		while (pJoint != nullptr) {
			Rval.push_back(pJoint);
			pJoint = pJoint->pParent;
		}

		return Rval;
	}//buildKinematicChainToRoot

	void InverseKinematicsController::update(float FPSScale) {

		forwardKinematics(m_pRoot, m_GlobalActorPosition, m_GlobalActorRotation);
		
		// update each end-effector
		for (auto* i : m_Joints) {
			if (i->pEndEffectorData == nullptr) continue;

			std::vector<Joint*> KinematicChain = buildKinematicChainToRoot(i);
						
			TopDownCCDIK(i, &KinematicChain);

		}//for[each end-effector]
	}//update

	void InverseKinematicsController::applyAnimation(bool UpdateUBO) {
		
		updateSkinningMatrices(m_pRoot, Matrix4f::Identity());
		
		if (UpdateUBO) {
			for (uint32_t i = 0; i < m_Joints.size(); ++i) m_UBO.skinningMatrix(i, m_Joints[i]->SkinningMatrix);
		}

	}//applyAnimation

	UBOBoneData* InverseKinematicsController::ubo(void) {
		return &m_UBO;
	}//ubo

	void InverseKinematicsController::TopDownCCDIK(Joint* pEndEffector, std::vector<Joint*>* pChain) {
		if (pEndEffector == nullptr) throw NullpointerExcept("pEndEffector");
		if (pChain == nullptr) throw NullpointerExcept("pChain");

		for (int32_t i = 0; i < m_MaxIterations; ++i) {

			// for each joint in constructed kinematic chain of current end-effector
			for (int32_t k = 0; k < pChain->size(); ++k) {
								
				Joint* pCurrentJoint = (*pChain)[k];

				Vector3f TargetPos = pEndEffector->pEndEffectorData->Target;
				Vector3f EffectorPos = pEndEffector->GlobalPosition;			// get current end-effector position in world space
				Vector3f JointPos = pCurrentJoint->GlobalPosition;				// get current joint position in world space
				Vector3f JointToTarget = TargetPos - JointPos;					// directional vector in world space: current joint position -> target position
				Vector3f JointToEffector = EffectorPos - JointPos;				// directional vector in world space: current joint position -> current end-effector position
				
				JointToEffector.normalize();
				JointToTarget.normalize();

				// compute unconstrained rotation quaternion to align both directional vectors in world space
				Quaternionf GlobalIncrement;
				GlobalIncrement.setFromTwoVectors(JointToEffector, JointToTarget);

				// apply rotation to joint and update affected global rotations and positions
				Vector3f GlobalParentPosition = m_GlobalActorPosition;
				Quaternionf GlobalParentRotation = m_GlobalActorRotation;

				if (pCurrentJoint->pParent != nullptr) {
					GlobalParentPosition = pCurrentJoint->pParent->GlobalPosition;
					GlobalParentRotation = pCurrentJoint->pParent->GlobalRotation;
				}

				pCurrentJoint->LocalRotation = GlobalParentRotation.inverse() * (GlobalIncrement * pCurrentJoint->GlobalRotation);
				
				Vector3f LastEndEffectorPos = pEndEffector->GlobalPosition;
				forwardKinematics(pCurrentJoint, GlobalParentPosition, GlobalParentRotation);

				float DistToTarget = (TargetPos - pEndEffector->GlobalPosition).norm();
				if (DistToTarget < 0.0001f) return; // terminate iterations -> end-effector reached target

				float EndEffectorMovement = (LastEndEffectorPos - pEndEffector->GlobalPosition).norm();
				if (EndEffectorMovement < 0.0001f) return; // terminate iterations -> can't improve end-effector position

			}//for [each joint in chain]

		}//for [m_MaxIterations of IK]

	}//TopDownCCDIK

	void InverseKinematicsController::forwardKinematics(Joint* pJoint, Vector3f ParentPosition, Quaternionf ParentRotation) {
		if (nullptr == pJoint) throw NullpointerExcept("pJoint");

		pJoint->GlobalPosition = ParentPosition + (ParentRotation * (m_GlobalActorScaling.cwiseProduct(pJoint->LocalPosition)));
		pJoint->GlobalRotation = ParentRotation * pJoint->LocalRotation;

		for (auto i : pJoint->Children) forwardKinematics(i, pJoint->GlobalPosition, pJoint->GlobalRotation);
	}//forwardKinematics

	void InverseKinematicsController::updateSkinningMatrices(Joint* pJoint, Eigen::Matrix4f ParentTransform) {
		if (nullptr == pJoint) throw NullpointerExcept("pJoint");
		const Matrix4f R = CForgeMath::rotationMatrix(pJoint->LocalRotation);
		const Matrix4f T = CForgeMath::translationMatrix(pJoint->LocalPosition);
		const Matrix4f S = CForgeMath::scaleMatrix(pJoint->LocalScale);
		const Matrix4f JointTransform = T * R * S;

		Matrix4f LocalTransform = ParentTransform * JointTransform;
		pJoint->SkinningMatrix = LocalTransform * pJoint->OffsetMatrix;

		for (auto i : pJoint->Children) updateSkinningMatrices(i, LocalTransform);
	}//updateSkinningMatrices

	void InverseKinematicsController::retrieveSkinningMatrices(std::vector<Matrix4f>* pSkinningMats) {
		if (nullptr == pSkinningMats) throw NullpointerExcept("pSkinningMats");
		pSkinningMats->clear();
		for (auto i : m_Joints) pSkinningMats->push_back(i->SkinningMatrix);
	}//retrieveSkinningMatrices

	std::vector<InverseKinematicsController::SkeletalJoint*> InverseKinematicsController::retrieveSkeleton(void) const {
		std::vector<SkeletalJoint*> Rval;

		for (auto i : m_Joints) {
			SkeletalJoint* pNewJoint = new SkeletalJoint();
			pNewJoint->ID = i->ID;
			pNewJoint->Name = i->Name;
			pNewJoint->OffsetMatrix = i->OffsetMatrix;
			pNewJoint->LocalPosition = i->LocalPosition;
			pNewJoint->LocalRotation = i->LocalRotation;
			pNewJoint->LocalScale = i->LocalScale;
			pNewJoint->SkinningMatrix = i->SkinningMatrix;

			pNewJoint->Parent = (i->pParent == nullptr) ? -1 : i->pParent->ID;
			for (auto k : i->Children) pNewJoint->Children.push_back(k->ID);
			Rval.push_back(pNewJoint);
		}
		return Rval;
	}//retrieveSkeleton

	void InverseKinematicsController::updateSkeletonValues(std::vector<InverseKinematicsController::SkeletalJoint*>* pSkeleton) {
		if (nullptr == pSkeleton) throw NullpointerExcept("pSkeleton");

		for (auto i : (*pSkeleton)) {
			i->OffsetMatrix = m_Joints[i->ID]->OffsetMatrix;
			i->LocalPosition = m_Joints[i->ID]->LocalPosition;
			i->LocalRotation = m_Joints[i->ID]->LocalRotation;
			i->LocalScale = m_Joints[i->ID]->LocalScale;
			i->SkinningMatrix = m_Joints[i->ID]->SkinningMatrix;
		}
	}//updateSkeleton

	void InverseKinematicsController::globalActorPosition(Vector3f Position) {
		if (nullptr == m_pRoot) throw NullpointerExcept("m_pRoot");

		m_GlobalActorPosition = Position;
		forwardKinematics(m_pRoot, Position, m_GlobalActorRotation);
	}//globalActorPosition

	void InverseKinematicsController::globalActorRotation(Quaternionf Rotation) {
		if (nullptr == m_pRoot) throw NullpointerExcept("m_pRoot");

		m_GlobalActorRotation = Rotation;
		forwardKinematics(m_pRoot, m_GlobalActorPosition, Rotation);
	}//globalActorRotation

	void InverseKinematicsController::globalActorScaling(Vector3f Scaling) {
		if (nullptr == m_pRoot) throw NullpointerExcept("m_pRoot");

		m_GlobalActorScaling = Scaling;
		forwardKinematics(m_pRoot, m_GlobalActorPosition, m_GlobalActorRotation);
	}//globalActorScaling

	Vector3f InverseKinematicsController::globalActorPosition(void) const {
		return m_GlobalActorPosition;
	}//globalActorPosition

	Quaternionf InverseKinematicsController::globalActorRotation(void) const {
		return m_GlobalActorRotation;
	}//globalActorRotation

	Eigen::Vector3f InverseKinematicsController::globalActorScaling(void) const {
		return m_GlobalActorScaling;
	}//globalActorScaling

	std::vector<InverseKinematicsController::SkeletalEndEffector*> InverseKinematicsController::retrieveEndEffectors(void) const {
		std::vector<SkeletalEndEffector*> Rval;

		for (auto i : m_Joints) {
			if (i->pEndEffectorData == nullptr) continue;

			SkeletalEndEffector* pNewEffector = new SkeletalEndEffector();
			pNewEffector->JointID = i->ID;
			pNewEffector->JointName = i->Name;
			pNewEffector->Target = i->pEndEffectorData->Target;

			Rval.push_back(pNewEffector);
		}
		return Rval;
	}//retrieveEndEffectors

	void InverseKinematicsController::endEffectorTarget(const SkeletalEndEffector* pModifiedEndEffector) {
		if (nullptr == pModifiedEndEffector) throw NullpointerExcept("pModifiedEndEffector");

		for (auto* i : m_Joints) {
			if (i->ID == pModifiedEndEffector->JointID) {
				i->pEndEffectorData->Target = pModifiedEndEffector->Target;
				break;
			}
		}
	}//endEffectorTarget

}//name space