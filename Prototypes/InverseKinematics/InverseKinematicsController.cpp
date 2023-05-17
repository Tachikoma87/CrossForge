#include "InverseKinematicsController.h"
#include <crossforge/Graphics/Shader/SShaderManager.h>
#include <crossforge/Math/CForgeMath.h>
#include <crossforge/Core/SLogger.h>

#include <fstream>

using namespace Eigen;
using namespace std;

namespace CForge {

	InverseKinematicsController::InverseKinematicsController(void): CForgeObject("InverseKinematicsController") {
		m_pRoot = nullptr;
		m_pHead = nullptr;

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
	void InverseKinematicsController::init(T3DMesh<float>* pMesh, std::string ConfigFilepath, Vector3f GlobalActorPosition, Quaternionf GlobalActorRotation, Vector3f GlobalActorScaling) {
		clear();

		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->boneCount() == 0) throw CForgeExcept("Mesh has no bones!");
		if (pMesh->skeletalAnimationCount() == 0) throw CForgeExcept("Mesh has no animation data!"); // use first keyframe of first animation as rest pose of skeleton for now
		
		// create joints and copy data
		std::ifstream f(ConfigFilepath);
		const nlohmann::json ConfigData = nlohmann::json::parse(f);
		const nlohmann::json& ConstraintData = ConfigData.at("JointConstraints");
		T3DMesh<float>::SkeletalAnimation* pAnimation = pMesh->getSkeletalAnimation(0);

		for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
			const T3DMesh<float>::Bone* pRef = pMesh->getBone(i);

			Joint* pJoint = new Joint();
			pJoint->ID = pRef->ID;
			pJoint->Name = pRef->Name;
			pJoint->LocalPosition = pAnimation->Keyframes[i]->Positions[0];
			pJoint->LocalRotation = pAnimation->Keyframes[i]->Rotations[0];
			pJoint->LocalScale = pAnimation->Keyframes[i]->Scalings[0];
			pJoint->GlobalPosition = Vector3f::Zero(); // computed after joint hierarchy has been constructed
			pJoint->GlobalRotation = Quaternionf::Identity(); // computed after joint hierarchy has been constructed
			pJoint->OffsetMatrix = pRef->OffsetMatrix;
			pJoint->SkinningMatrix = Matrix4f::Identity(); // computed during applyAnimation()
			
			// create user defined joint constraints
			const nlohmann::json& JointData = ConstraintData.at(pJoint->Name);
			std::string Type = JointData.at("ConstraintType").get<std::string>();

			if (Type == "Unconstrained") pJoint->ConstraintType = UNCONSTRAINED;

			if (Type == "Hinge") {
				pJoint->ConstraintType = HINGE;
				//TODO: set constraint properties
				//...
			}

			if (Type == "BallAndSocket") {
				pJoint->ConstraintType = BALL_AND_SOCKET;
				//TODO: set constraint properties
				//...
			}

			//if (Type == "...") ...

			m_Joints.push_back(pJoint);	
		}//for[bones]
		
		// copy structure
		for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
			const T3DMesh<float>::Bone* pRef = pMesh->getBone(i);
			Joint* pJoint = m_Joints[i];

			pJoint->pParent = (pRef->pParent != nullptr) ? m_Joints[pRef->pParent->ID] : nullptr;

			for (uint32_t k = 0; k < pRef->Children.size(); ++k) pJoint->Children.push_back(m_Joints[pRef->Children[k]->ID]);
		}//for[bones]

		// find root bone
		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			if (m_Joints[i]->pParent == nullptr) {
				m_pRoot = m_Joints[i];
				break;
			}
		}//for[joints]

		// create user defined skeleton segments 
		const nlohmann::json& SegmentData = ConfigData.at("SkeletonSegments");

		buildKinematicChain(RIGHT_ARM, SegmentData.at("RightArm"));
		buildKinematicChain(LEFT_ARM, SegmentData.at("LeftArm"));
		buildKinematicChain(RIGHT_LEG, SegmentData.at("RightLeg"));
		buildKinematicChain(LEFT_LEG, SegmentData.at("LeftLeg"));
		buildKinematicChain(SPINE, SegmentData.at("Spine"));

		std::string GazeJointName = SegmentData.at("GazeJoint").get<std::string>();
		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			if (m_Joints[i]->Name == GazeJointName) {
				m_pHead = new HeadJoint();
				m_pHead->pJoint = m_Joints[i];
				break;
			}
		}//for[joints]

		if (m_pHead->pJoint == nullptr) throw NullpointerExcept("m_pHead->pJoint");

		// initialize global positions and rotations of all joints given the world space transformation of the actor
		m_GlobalActorPosition = GlobalActorPosition;
		m_GlobalActorRotation = GlobalActorRotation;
		m_GlobalActorScaling = GlobalActorScaling;
		forwardKinematics(m_pRoot, GlobalActorPosition, GlobalActorRotation);
			
		// initialize end-effector targets
		for (auto& c : m_KinematicChains) {
			auto& Chain = c.second;
			Chain.Target = Chain.Joints.front()->GlobalPosition; // first joint is end-effector
		}//for[kinematic chains]
		
		// initialize gaze direction and target using the default coordinate system of glTF 2.0 and assuming that the character is always looking straight ahead in bind pose:  
		// -> from https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#coordinate-system-and-units: "glTF uses a right-handed coordinate system. glTF defines +Y as 
		// up, +Z as forward, and -X as right; the front of a glTF asset faces +Z."
		m_pHead->CurrentGlobalDir = (m_GlobalActorRotation * Vector3f(0.0f, 0.0f, 1.0f)).normalized();
		m_pHead->Target = m_pHead->pJoint->GlobalPosition + m_pHead->CurrentGlobalDir;
		
		// initialize UBO
		m_UBO.init(m_Joints.size());

		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			m_UBO.skinningMatrix(i, Matrix4f::Identity());
		}//for[joints]

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
		
		if (m_pHead != nullptr) {
			m_pHead->pJoint = nullptr;
			delete m_pHead;
		}
		m_pHead = nullptr;

		m_Joints.clear();
		m_KinematicChains.clear();
		
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
	
	void InverseKinematicsController::initJointConstraints(const nlohmann::json& ConstraintDefinitions) {
		
	}//initJointConstraints

	void InverseKinematicsController::buildKinematicChain(SkeletalSegment SegmentID, const nlohmann::json& ChainData) {
		m_KinematicChains.try_emplace(SegmentID);
		KinematicChain& Chain = m_KinematicChains.at(SegmentID);

		std::string RootName = ChainData.at("Root").get<std::string>();
		std::string EndEffectorName = ChainData.at("EndEffector").get<std::string>();

		// fill chain in order end-effector -> chain root
		Joint* pCurrent = nullptr; 
		for (Joint* i : m_Joints) { 
			if (i->Name == EndEffectorName) {
				pCurrent = i;
				break;
			}	
		}
		if (pCurrent == nullptr) throw NullpointerExcept("pCurrent"); // couldn't find the end-effector joint

		Joint* pEnd = nullptr; 
		for (Joint* i : m_Joints) { 
			if (i->Name == RootName) { 
				pEnd = i; 
				break; 
			} 
		}
		if (pEnd == nullptr) throw NullpointerExcept("pEnd"); // couldn't find the root joint
				
		while (pCurrent != pEnd->pParent) { //TODO: test if this works properly!
			if (pCurrent == m_pRoot && pEnd != m_pRoot) throw CForgeExcept("Reached root joint of skeleton without finding root joint of kinematic chain!");

			Chain.Joints.push_back(pCurrent);
			pCurrent = pCurrent->pParent;
		}

		if (Chain.Joints.size() < 2) throw CForgeExcept("Initialization of kinematic chain failed!");
	}//buildKinematicChain

	void InverseKinematicsController::update(float FPSScale) {

		forwardKinematics(m_pRoot, m_GlobalActorPosition, m_GlobalActorRotation);

		rotateGaze();
		ikCCD(RIGHT_ARM);
		ikCCD(LEFT_ARM);
		ikCCD(RIGHT_LEG);
		ikCCD(LEFT_LEG);
		ikCCD(SPINE);
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

	void InverseKinematicsController::ikCCD(SkeletalSegment SegmentID) {
		KinematicChain& Chain = m_KinematicChains.at(SegmentID);
		std::vector<Joint*>& Joints = Chain.Joints;
		Joint* pEndEffector = Joints[0];

		for (int32_t i = 0; i < m_MaxIterations; ++i) {
			for (int32_t k = 1; k < Joints.size(); ++k) {
								
				Joint* pCurrentJoint = Joints[k];

				Vector3f TargetPos = Chain.Target;
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
				constrainLocalRotation(pCurrentJoint);

				Vector3f LastEndEffectorPos = pEndEffector->GlobalPosition;
				forwardKinematics(pCurrentJoint, GlobalParentPosition, GlobalParentRotation);

				float DistToTarget = (TargetPos - pEndEffector->GlobalPosition).norm();
				if (DistToTarget < 0.00001f) return; // terminate iterations -> end-effector reached target

				float EndEffectorMovement = (LastEndEffectorPos - pEndEffector->GlobalPosition).norm();
				if (EndEffectorMovement < 0.00001f) return; // terminate iterations -> can't improve end-effector position

			}//for[each joint in chain]
		}//for[m_MaxIterations]
	}//ikCCD

	void InverseKinematicsController::rotateGaze(void) {
		Joint* pJoint = m_pHead->pJoint;

		Vector3f CurrentGlobalDir = m_pHead->CurrentGlobalDir; // global gaze direction before update (expected to be normalized; new values will be normalized when rotation updates happen)
		Vector3f GlobalTargetDir = (m_pHead->Target - pJoint->GlobalPosition).normalized();

		if (std::abs(1.0f - CurrentGlobalDir.dot(GlobalTargetDir) > 0.00001f)) {
			Vector3f CurrentLocalDir = pJoint->GlobalRotation.inverse() * CurrentGlobalDir; // local gaze direction in joint space before update
						
			// compute unconstrained rotation quaternion to align both directional vectors in world space
			Quaternionf GlobalIncrement;
			GlobalIncrement.setFromTwoVectors(CurrentGlobalDir, GlobalTargetDir);

			// compute local joint rotation and constrain		
			Quaternionf GlobalParentRotation = pJoint->pParent->GlobalRotation;
			pJoint->LocalRotation = GlobalParentRotation.inverse() * (GlobalIncrement * pJoint->GlobalRotation);
			constrainLocalRotation(pJoint);

			// compute new global joint rotation and apply to gaze direction
			forwardKinematics(pJoint, pJoint->pParent->GlobalPosition, GlobalParentRotation);

			// apply new global rotation of pJoint to old gaze direction in joint space to compute the new (normalized) gaze direction in global space
			m_pHead->CurrentGlobalDir = (pJoint->GlobalRotation * CurrentLocalDir).normalized();
		}
	}//rotateGaze

	void InverseKinematicsController::constrainLocalRotation(Joint* pJoint) {
		if (pJoint->ConstraintType == UNCONSTRAINED) return; // do nothing

		if (pJoint->ConstraintType == HINGE) {
			
			//TODO
			
			// constrain rotation about the hinge axis:
			// ==> apply unconstrained local rotation to hinge axis of joint ==> RotatedAxis = pJoint->LocalRotation * pJoint->HingeAxis
			// ==> compute rotation from RotatedAxis to the correct (original) pJoint->HingeAxis ==> setFromTwoVectors(RotatedAxis, pJoint->HingeAxis)
			// ==> apply rotation to pJoint->LocalRotation
			
			// enforce angle limits:
			// ==> using Eigen::AngleAxisf:
			//		==> AngleAxisf RotAngleAxis = pJoint->LocalRotation (this assignment automatically decomposes the quaternion into an axis vector and an angle value)
			//		==> clamp Angle to min/max values stored in pJoint
			//		==> reconstruct quaternion using Eigen::AngleAxisf and assign as new pJoint->LocalRotation

		}
		else if (pJoint->ConstraintType == BALL_AND_SOCKET) {
			//TODO
			//pJoint->LocalRotation = ...
		}
		else /*if (pJoint->ConstraintType == ...)*/ {
			//TODO
			//pJoint->LocalRotation = ...
		}
	}//constrainLocalRotation

	void InverseKinematicsController::forwardKinematics(Joint* pJoint, Vector3f ParentPosition, Quaternionf ParentRotation) {
		if (nullptr == pJoint) throw NullpointerExcept("pJoint");

		pJoint->GlobalPosition = ParentPosition + (ParentRotation * (m_GlobalActorScaling.cwiseProduct(pJoint->LocalPosition)));
		pJoint->GlobalRotation = ParentRotation * pJoint->LocalRotation;

		for (auto i : pJoint->Children) forwardKinematics(i, pJoint->GlobalPosition, pJoint->GlobalRotation);
	}//forwardKinematics

	void InverseKinematicsController::updateSkinningMatrices(Joint* pJoint, Matrix4f ParentTransform) {
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
	}//updateSkeletonValues

	std::vector<InverseKinematicsController::SkeletalEndEffector*> InverseKinematicsController::retrieveEndEffectors(void) const {
		std::vector<SkeletalEndEffector*> Rval;
		SkeletalEndEffector* pNewEffector = nullptr;

		// limbs and spine
		for (const auto& Chain : m_KinematicChains) {
			Joint* pEndEffector = *Chain.second.Joints.begin();

			pNewEffector = new SkeletalEndEffector();
			pNewEffector->JointID = pEndEffector->ID;
			pNewEffector->JointName = pEndEffector->Name;
			pNewEffector->Segment = Chain.first;
			pNewEffector->Target = Chain.second.Target;
			Rval.push_back(pNewEffector);
		}

		// gaze
		pNewEffector = new SkeletalEndEffector();
		pNewEffector->JointID = m_pHead->pJoint->ID;
		pNewEffector->JointName = m_pHead->pJoint->Name;
		pNewEffector->Segment = HEAD;
		pNewEffector->Target = m_pHead->Target;
		Rval.push_back(pNewEffector);

		return Rval;
	}//retrieveEndEffectors

	void InverseKinematicsController::updateEndEffectorValues(std::vector<SkeletalEndEffector*>* pEndEffectors) {
		if (nullptr == pEndEffectors) throw NullpointerExcept("pEndEffectors");

		for (auto i : (*pEndEffectors)) {
			switch (i->Segment) {
			case HEAD: {
				i->Target = m_pHead->Target;
				break;
			}
			case RIGHT_ARM:
			case LEFT_ARM:
			case RIGHT_LEG:
			case LEFT_LEG:
			case SPINE: {
				i->Target = m_KinematicChains.at(i->Segment).Target;
				break;
			}
			}
		}
	}//updateEndEffectorValues

	void InverseKinematicsController::endEffectorTarget(SkeletalSegment SegmentID, Vector3f NewTarget) {
		switch (SegmentID) {
		case HEAD: {
			m_pHead->Target = NewTarget;
			break;
		}
		case RIGHT_ARM:
		case LEFT_ARM:
		case RIGHT_LEG:
		case LEFT_LEG:
		case SPINE: {
			m_KinematicChains.at(SegmentID).Target = NewTarget;
			break;
		}
		}
	}//endEffectorTarget

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

	Vector3f InverseKinematicsController::globalActorScaling(void) const {
		return m_GlobalActorScaling;
	}//globalActorScaling
}//name space