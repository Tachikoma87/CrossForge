#include "InverseKinematicsController.h"
#include <crossforge/Graphics/Shader/SShaderManager.h>
#include <crossforge/Math/CForgeMath.h>
#include <crossforge/Core/SLogger.h>

#include <fstream>

using namespace Eigen;

namespace CForge {

	InverseKinematicsController::InverseKinematicsController(void): CForgeObject("InverseKinematicsController") {
		m_pRoot = nullptr;
		m_pHead = nullptr;

		m_MaxIterations = 50;
		
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
	void InverseKinematicsController::init(T3DMesh<float>* pMesh, std::string ConfigFilepath) {
		clear();

		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->boneCount() == 0) throw CForgeExcept("Mesh has no bones!");
		if (pMesh->skeletalAnimationCount() == 0) throw CForgeExcept("Mesh has no animation data!"); // use first keyframe of first animation as rest pose of skeleton for now
		
		std::ifstream f(ConfigFilepath);
		const nlohmann::json ConfigData = nlohmann::json::parse(f);
		
		initJointProperties(pMesh, ConfigData.at("JointLimits"));
		
		initSkeletonStructure(pMesh, ConfigData.at("SkeletonStructure"));
		
		forwardKinematics(m_pRoot); // initialize global positions and rotations of all joints
		
		initEndEffectorPoints(ConfigData.at("EndEffectorProperties"));
		
		initTargetPoints();
		
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
		for (auto& i : m_Joints) if (nullptr != i) {
			if (nullptr != i->pEndEffectorData) delete i->pEndEffectorData;
			if (nullptr != i->pLimits) delete i->pLimits;
			delete i;
		}
		
		m_pHead = nullptr;

		m_Joints.clear();
		m_JointChains.clear();
		
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
	
	void InverseKinematicsController::initJointProperties(T3DMesh<float>* pMesh, const nlohmann::json& ConstraintData) {
		T3DMesh<float>::SkeletalAnimation* pAnimation = pMesh->getSkeletalAnimation(0); // used as initial pose of skeleton

		for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
			const T3DMesh<float>::Bone* pRef = pMesh->getBone(i);
			Joint* pJoint = new Joint();

			pJoint->ID = pRef->ID;
			pJoint->Name = pRef->Name;
			pJoint->LocalPosition = pAnimation->Keyframes[i]->Positions[0];
			pJoint->LocalRotation = pAnimation->Keyframes[i]->Rotations[0].normalized();
			pJoint->LocalScale = pAnimation->Keyframes[i]->Scalings[0];
			pJoint->GlobalPosition = Vector3f::Zero(); // computed after joint hierarchy has been constructed
			pJoint->GlobalRotation = Quaternionf::Identity(); // computed after joint hierarchy has been constructed
			pJoint->OffsetMatrix = pRef->OffsetMatrix;
			pJoint->SkinningMatrix = Matrix4f::Identity(); // computed during applyAnimation()
			
			// create user defined joint constraints
			const nlohmann::json& JointData = ConstraintData.at(pJoint->Name);
			std::string Type = JointData.at("Type").get<std::string>();

			if (Type == "Unconstrained") pJoint->pLimits = nullptr;

			if (Type == "Hinge") {				
				std::string Hinge = JointData.at("HingeAxis").get<std::string>();
				std::string Forward = JointData.at("BoneForward").get<std::string>();

				if (Hinge == Forward) throw CForgeExcept("JointLimits for '" + pJoint->Name + "': HingeAxis and BoneForward cannot be the same joint axis!");

				Vector3f HingeAxis = Vector3f::Zero();
				Vector3f BoneForward = Vector3f::Zero();

				if (Hinge == "x") HingeAxis = Vector3f::UnitX();
				else if (Hinge == "y") HingeAxis = Vector3f::UnitY();
				else if (Hinge == "z") HingeAxis = Vector3f::UnitZ();
				else throw CForgeExcept("JointLimits for '" + pJoint->Name + "': HingeAxis must be 'x', 'y' or 'z'!");

				if (Forward == "x") BoneForward = Vector3f::UnitX();
				else if (Forward == "y") BoneForward = Vector3f::UnitY();
				else if (Forward == "z") BoneForward = Vector3f::UnitZ();
				else throw CForgeExcept("JointLimits for '" + pJoint->Name + "': BoneForward must be 'x', 'y' or 'z'!");

				float MinRad = CForgeMath::degToRad(JointData.at("MinAngleDegrees").get<float>());
				float MaxRad = CForgeMath::degToRad(JointData.at("MaxAngleDegrees").get<float>());

				HingeLimits* pNewLimits = new HingeLimits(pJoint->LocalRotation, HingeAxis, BoneForward, MinRad, MaxRad);
				pJoint->pLimits = pNewLimits;
			}
			
			if (Type == "SwingXZTwistY") {
				float MinTwist = CForgeMath::degToRad(JointData.at("MinTwist").get<float>());
				float MaxTwist = CForgeMath::degToRad(JointData.at("MaxTwist").get<float>());
				float MinXSwing = CForgeMath::degToRad(JointData.at("MinXSwing").get<float>());
				float MaxXSwing = CForgeMath::degToRad(JointData.at("MaxXSwing").get<float>());
				float MinZSwing = CForgeMath::degToRad(JointData.at("MinZSwing").get<float>());
				float MaxZSwing = CForgeMath::degToRad(JointData.at("MaxZSwing").get<float>());
				
				SwingXZTwistYLimits* pNewLimits = new SwingXZTwistYLimits(pJoint->LocalRotation, MinXSwing, MaxXSwing, MinZSwing, MaxZSwing, MinTwist, MaxTwist);
				pJoint->pLimits = pNewLimits;
			}

			if (Type == "SwingXTwistY") {
				float MinTwist = CForgeMath::degToRad(JointData.at("MinTwist").get<float>());
				float MaxTwist = CForgeMath::degToRad(JointData.at("MaxTwist").get<float>());
				float MinSwing = CForgeMath::degToRad(JointData.at("MinSwing").get<float>());
				float MaxSwing = CForgeMath::degToRad(JointData.at("MaxSwing").get<float>());
				
				SwingXTwistYLimits* pNewLimits = new SwingXTwistYLimits(pJoint->LocalRotation, MinSwing, MaxSwing, MinTwist, MaxTwist);
				pJoint->pLimits = pNewLimits;
			}

			if (Type == "SwingZTwistY") {
				float MinTwist = CForgeMath::degToRad(JointData.at("MinTwist").get<float>());
				float MaxTwist = CForgeMath::degToRad(JointData.at("MaxTwist").get<float>());
				float MinSwing = CForgeMath::degToRad(JointData.at("MinSwing").get<float>());
				float MaxSwing = CForgeMath::degToRad(JointData.at("MaxSwing").get<float>());

				SwingZTwistYLimits* pNewLimits = new SwingZTwistYLimits(pJoint->LocalRotation, MinSwing, MaxSwing, MinTwist, MaxTwist);
				pJoint->pLimits = pNewLimits;
			}

			if (Type == "SwingXYTwistZ") {
				float MinTwist = CForgeMath::degToRad(JointData.at("MinTwist").get<float>());
				float MaxTwist = CForgeMath::degToRad(JointData.at("MaxTwist").get<float>());
				float MinXSwing = CForgeMath::degToRad(JointData.at("MinXSwing").get<float>());
				float MaxXSwing = CForgeMath::degToRad(JointData.at("MaxXSwing").get<float>());
				float MinYSwing = CForgeMath::degToRad(JointData.at("MinYSwing").get<float>());
				float MaxYSwing = CForgeMath::degToRad(JointData.at("MaxYSwing").get<float>());
				
				SwingXYTwistZLimits* pNewLimits = new SwingXYTwistZLimits(pJoint->LocalRotation, MinXSwing, MaxXSwing, MinYSwing, MaxYSwing, MinTwist, MaxTwist);
				pJoint->pLimits = pNewLimits;
			}

			//if (Type == "SwingXTwistZ") ...
			//if (Type == "SwingYTwistZ") ...
			//if (Type == "SwingYZTwistX") ...
			//if (Type == "SwingYTwistX") ...
			//if (Type == "SwingZTwistX") ...

			m_Joints.push_back(pJoint);
		}//for[bones]
	}//initJointProperties

	void InverseKinematicsController::initSkeletonStructure(T3DMesh<float>* pMesh, const nlohmann::json& StructureData) {
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
		buildKinematicChain(SPINE, StructureData.at("Spine"));
		buildKinematicChain(RIGHT_ARM, StructureData.at("RightArm"));
		buildKinematicChain(LEFT_ARM, StructureData.at("LeftArm"));
		buildKinematicChain(RIGHT_LEG, StructureData.at("RightLeg"));
		buildKinematicChain(LEFT_LEG, StructureData.at("LeftLeg"));
		
		// designate joint for gaze
		std::string GazeJointName = StructureData.at("Gaze").at("Joint").get<std::string>();
		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			if (m_Joints[i]->Name == GazeJointName) {
				m_pHead = m_Joints[i];
				break;
			}
		}//for[joints]

		if (m_pHead == nullptr) throw NullpointerExcept("m_pHead");
	}//initSkeletonStructure

	void InverseKinematicsController::buildKinematicChain(SkeletalSegment SegmentID, const nlohmann::json& ChainData) {
		m_JointChains.try_emplace(SegmentID);
		std::vector<Joint*>& Chain = m_JointChains.at(SegmentID);

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
				
		while (pCurrent != pEnd->pParent) {
			if (pCurrent == m_pRoot && pEnd != m_pRoot) throw CForgeExcept("Reached root joint of skeleton without finding root joint of chain!");

			Chain.push_back(pCurrent);
			pCurrent = pCurrent->pParent;
		}

		if (Chain.size() < 2) throw CForgeExcept("Initialization of chain failed!");
	}//buildKinematicChain

	void InverseKinematicsController::initEndEffectorPoints(const nlohmann::json& EndEffectorPropertiesData) {
		for (const auto& el : EndEffectorPropertiesData.items()) {
			if (el.key() == "Gaze") {
				if (m_pHead == nullptr) throw NullpointerExcept("m_pHead");

				EndEffectorData* pEffData = new EndEffectorData();

				pEffData->LocalEndEffectorPoints.resize(3, 1);
				pEffData->GlobalEndEffectorPoints.resize(3, 1);

				float FwdX = el.value().at("Forward").at("x").get<float>();
				float FwdY = el.value().at("Forward").at("y").get<float>();
				float FwdZ = el.value().at("Forward").at("z").get<float>();
				Vector3f Forward = Vector3f(FwdX, FwdY, FwdZ);

				pEffData->GlobalEndEffectorPoints.col(0) = m_pHead->GlobalPosition + Forward;
				pEffData->LocalEndEffectorPoints.col(0) = m_pHead->GlobalRotation.conjugate() * Forward;

				m_pHead->pEndEffectorData = pEffData;
			}
			else {
				SkeletalSegment Seg = SkeletalSegment::NONE;
				if (el.key() == "Spine") Seg = SkeletalSegment::SPINE;
				if (el.key() == "RightArm") Seg = SkeletalSegment::RIGHT_ARM;
				if (el.key() == "LeftArm") Seg = SkeletalSegment::LEFT_ARM;
				if (el.key() == "RightLeg") Seg = SkeletalSegment::RIGHT_LEG;
				if (el.key() == "LeftLeg") Seg = SkeletalSegment::LEFT_LEG;
				if (Seg == SkeletalSegment::NONE) throw CForgeExcept("EndEffectorProperties: unknown skeleton segment name '" + el.key() + "'");

				const nlohmann::json& Points = el.value().at("Points");
				Joint* pEff = m_JointChains.at(Seg).front();
				EndEffectorData* pEffData = new EndEffectorData();

				pEffData->LocalEndEffectorPoints.resize(3, Points.size());
				pEffData->GlobalEndEffectorPoints.resize(3, Points.size());

				for (const auto& el : Points.items()) {
					int32_t PointIdx = std::stoi(el.key());
					pEffData->GlobalEndEffectorPoints(0, PointIdx) = el.value().at("x").get<float>();
					pEffData->GlobalEndEffectorPoints(1, PointIdx) = el.value().at("y").get<float>();
					pEffData->GlobalEndEffectorPoints(2, PointIdx) = el.value().at("z").get<float>();
				}

				for (int32_t i = 0; i < pEffData->LocalEndEffectorPoints.cols(); ++i) {
					pEffData->LocalEndEffectorPoints.col(i) = pEff->GlobalRotation.conjugate() * (pEffData->GlobalEndEffectorPoints.col(i) - pEff->GlobalPosition);
				}

				pEff->pEndEffectorData = pEffData;
			}
		}
	}//initEndEffectorPoints

	void InverseKinematicsController::initTargetPoints(void) {
		m_pHead->pEndEffectorData->GlobalTargetPoints = m_pHead->pEndEffectorData->GlobalEndEffectorPoints;
		
		for (auto& c : m_JointChains) {
			Joint* pEff = c.second.front();
			pEff->pEndEffectorData->GlobalTargetPoints = pEff->pEndEffectorData->GlobalEndEffectorPoints;
			pEff->pEndEffectorData->GlobalTargetRotation = pEff->GlobalRotation;
		}
	}//initTargetPoints

	void InverseKinematicsController::update(float FPSScale) {

		forwardKinematics(m_pRoot);

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
		std::vector<Joint*>& Chain = m_JointChains.at(SegmentID);
		EndEffectorData* pEffData = Chain.front()->pEndEffectorData;
		Matrix3Xf LastEndEffectorPoints;

		for (int32_t i = 0; i < m_MaxIterations; ++i) {
			LastEndEffectorPoints = pEffData->GlobalEndEffectorPoints;

			for (int32_t k = 0; k < Chain.size(); ++k) {
				Joint* pCurrent = Chain[k];

				// compute unconstrained global rotation that best aligns position and orientation of end effector with desired target values
				Quaternionf GlobalIncrement = computeUnconstrainedGlobalRotation(pCurrent, pEffData);
				Quaternionf NewGlobalRotation = GlobalIncrement * pCurrent->GlobalRotation;
				
				// transform new global rotation to new local rotation
				Quaternionf NewLocalRotation = (pCurrent == m_pRoot) ? NewGlobalRotation : pCurrent->pParent->GlobalRotation.conjugate() * NewGlobalRotation;
				NewLocalRotation.normalize();

				// constrain new local rotation if joint is not unconstrained
				if (pCurrent->pLimits != nullptr) NewLocalRotation = pCurrent->pLimits->constrain(NewLocalRotation);

				// apply new local rotation to joint
				pCurrent->LocalRotation = NewLocalRotation;

				// update kinematic chain
				forwardKinematics(pCurrent);
			}//for[each joint in chain]

			// check for termination -> condition: end-effector has reached the targets position and orientation
			Matrix3Xf EffectorTargetDiff = pEffData->GlobalTargetPoints - pEffData->GlobalEndEffectorPoints;
			float DistError = EffectorTargetDiff.cwiseProduct(EffectorTargetDiff).sum() / float(EffectorTargetDiff.cols());
			if (DistError < 1e-6f) return;

			Matrix3Xf EffectorPosDiff = pEffData->GlobalEndEffectorPoints - LastEndEffectorPoints;
			float PosChangeError = EffectorPosDiff.cwiseProduct(EffectorPosDiff).sum() / float(EffectorPosDiff.cols());
			if (PosChangeError < 1e-6f) return;
		}//for[m_MaxIterations]
	}//ikCCD

	void InverseKinematicsController::rotateGaze(void) {
		Vector3f EPos = m_pHead->pEndEffectorData->GlobalEndEffectorPoints.col(0);
		Vector3f TPos = m_pHead->pEndEffectorData->GlobalTargetPoints.col(0);
		Vector3f CurrentDir = (EPos - m_pHead->GlobalPosition).normalized();
		Vector3f TargetDir = (TPos - m_pHead->GlobalPosition).normalized();

		if (std::abs(1.0f - CurrentDir.dot(TargetDir) > 1e-6f)) {			
			// compute unconstrained global rotation to align both directional vectors in world space
			Quaternionf GlobalIncrement;
			GlobalIncrement.setFromTwoVectors(CurrentDir, TargetDir);
			Quaternionf NewGlobalRotation = GlobalIncrement * m_pHead->GlobalRotation;
			
			// transform new global rotation to new local rotation
			Quaternionf NewLocalRotation = (m_pHead == m_pRoot) ? NewGlobalRotation : m_pHead->pParent->GlobalRotation.conjugate() * NewGlobalRotation;
			NewLocalRotation.normalize();

			// constrain new local rotation if joint is not unconstrained
			if (m_pHead->pLimits != nullptr) NewLocalRotation = m_pHead->pLimits->constrain(NewLocalRotation);

			// apply new local rotation to joint 
			m_pHead->LocalRotation = NewLocalRotation;

			// compute new global joint rotation and apply to gaze direction
			forwardKinematics(m_pHead);
		}
	}//rotateGaze

	Quaternionf InverseKinematicsController::computeUnconstrainedGlobalRotation(Joint* pJoint, InverseKinematicsController::EndEffectorData* pEffData) {

		//TODO: combine points of multiple end effectors and targets into 2 point clouds to compute CCD rotation for multiple end effectors?
		Matrix3Xf EndEffectorPoints = pEffData->GlobalEndEffectorPoints.colwise() - pJoint->GlobalPosition;
		Matrix3Xf TargetPoints = pEffData->GlobalTargetPoints.colwise() - pJoint->GlobalPosition;

#if 0

		// compute matrix W
		Matrix3f W = TargetPoints * EndEffectorPoints.transpose();

		// compute singular value decomposition
		JacobiSVD<Matrix3f> SVD(W, ComputeFullU | ComputeFullV);
		Matrix3f U = SVD.matrixU();
		Matrix3f V = SVD.matrixV();

		// compute rotation
		Matrix3f R = U * V.transpose();
		Quaternionf GlobalRotation(R);
		GlobalRotation.normalize();

#else

		// compute rotation using quaternion characteristic polynomial from: "Closed-form solution of absolute orientation using unit quaternions." - Berthold K. P. Horn, 1987

		//
		//			0	1	2
		//		0	Sxx	Sxy	Sxz
		// S =	1	Syx	Syy	Syz
		//		2	Szx	Szy	Szz
		//		
		Matrix3f S = EndEffectorPoints * TargetPoints.transpose();

		//
		// N = 
		//		Sxx + Syy + Szz		Syz - Szy			 Szx - Sxz			 Sxy - Syx
		//		Syz - Szy			Sxx - Syy - Szz		 Sxy + Syx			 Szx + Sxz
		//		Szx - Sxz			Sxy + Syx			-Sxx + Syy - Szz	 Syz + Szy
		//		Sxy - Syx			Szx + Sxz			 Syz + Szy			-Sxx - Syy + Szz
		//
		Matrix4f N = Matrix4f::Zero();

		N(0, 0) = S(0, 0) + S(1, 1) + S(2, 2);	//  Sxx + Syy + Szz
		N(0, 1) = S(1, 2) - S(2, 1);			//  Syz - Szy
		N(0, 2) = S(2, 0) - S(0, 2);			//  Szx - Sxz
		N(0, 3) = S(0, 1) - S(1, 0);			//  Sxy - Syx

		N(1, 0) = N(0, 1);						//  Syz - Szy
		N(1, 1) = S(0, 0) - S(1, 1) - S(2, 2);	//  Sxx - Syy - Szz
		N(1, 2) = S(0, 1) + S(1, 0);			//  Sxy + Syx
		N(1, 3) = S(2, 0) + S(0, 2);			//  Szx + Sxz

		N(2, 0) = N(0, 2);						//  Szx - Sxz
		N(2, 1) = N(1, 2);						//  Sxy + Syx
		N(2, 2) = -S(0, 0) + S(1, 1) - S(2, 2);	// -Sxx + Syy - Szz
		N(2, 3) = S(1, 2) + S(2, 1);			//  Syz + Szy

		N(3, 0) = N(0, 3);						//  Sxy - Syx
		N(3, 1) = N(1, 3);						//  Szx + Sxz
		N(3, 2) = N(2, 3);						//  Syz + Szy
		N(3, 3) = -S(0, 0) - S(1, 1) + S(2, 2);	// -Sxx - Syy + Szz
		
		Eigen::SelfAdjointEigenSolver<Matrix4f> Solver(4);
		Solver.compute(N);
		Vector4f BiggestEVec = Solver.eigenvectors().col(3); // last column of eigenvectors() matrix contains eigenvector of largest eigenvalue; that's supposed to equal the desired rotation quaternion
		Quaternionf GlobalRotation = Quaternionf(BiggestEVec(0), BiggestEVec(1), BiggestEVec(2), BiggestEVec(3)); 

#endif

		return GlobalRotation;
	}//computeUnconstrainedGlobalRotation

	void InverseKinematicsController::forwardKinematics(Joint* pJoint) {
		if (nullptr == pJoint) throw NullpointerExcept("pJoint");

		if (pJoint == m_pRoot) {
			pJoint->GlobalPosition = pJoint->LocalPosition;
			pJoint->GlobalRotation = pJoint->LocalRotation;
		}
		else {
			pJoint->GlobalPosition = (pJoint->pParent->GlobalRotation * pJoint->LocalPosition) + pJoint->pParent->GlobalPosition;
			pJoint->GlobalRotation = pJoint->pParent->GlobalRotation * pJoint->LocalRotation;
		}

		pJoint->GlobalRotation.normalize();

		if (pJoint->pEndEffectorData != nullptr) {
			auto& GlobalEndEffectorPoints = pJoint->pEndEffectorData->GlobalEndEffectorPoints;
			auto& LocalEndEffectorPoints = pJoint->pEndEffectorData->LocalEndEffectorPoints;

			for (int32_t i = 0; i < GlobalEndEffectorPoints.cols(); ++i) {
				GlobalEndEffectorPoints.col(i) = (pJoint->GlobalRotation * LocalEndEffectorPoints.col(i)) + pJoint->GlobalPosition;
			}
		}

		for (auto i : pJoint->Children) forwardKinematics(i);
	}//forwardKinematics

	void InverseKinematicsController::updateSkinningMatrices(Joint* pJoint, Matrix4f GlobalParentTransform) {
		if (nullptr == pJoint) throw NullpointerExcept("pJoint");
		const Matrix4f R = CForgeMath::rotationMatrix(pJoint->LocalRotation);
		const Matrix4f T = CForgeMath::translationMatrix(pJoint->LocalPosition);
		const Matrix4f S = CForgeMath::scaleMatrix(pJoint->LocalScale);
		const Matrix4f LocalJointTransform = T * R * S;

		Matrix4f GlobalJointTransform = GlobalParentTransform * LocalJointTransform;
		pJoint->SkinningMatrix = GlobalJointTransform * pJoint->OffsetMatrix;

		for (auto i : pJoint->Children) updateSkinningMatrices(i, GlobalJointTransform);
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
		
		for (const auto& c : m_JointChains) {
			SkeletalEndEffector* pEff = new SkeletalEndEffector();

			pEff->JointID = c.second.front()->ID;
			pEff->JointName = c.second.front()->Name;
			pEff->Segment = c.first;
			pEff->EndEffectorPoints = c.second.front()->pEndEffectorData->GlobalEndEffectorPoints;
			pEff->TargetPoints = c.second.front()->pEndEffectorData->GlobalTargetPoints;

			Rval.push_back(pEff);
		}

		// gaze
		SkeletalEndEffector* pEff = new SkeletalEndEffector();

		pEff->JointID = m_pHead->ID;
		pEff->JointName = m_pHead->Name;
		pEff->Segment = HEAD;
		pEff->EndEffectorPoints.resize(3, 1);
		pEff->EndEffectorPoints = m_pHead->pEndEffectorData->GlobalEndEffectorPoints;
		pEff->TargetPoints.resize(3, 1);
		pEff->TargetPoints = m_pHead->pEndEffectorData->GlobalTargetPoints;

		Rval.push_back(pEff);

		return Rval;
	}//retrieveEndEffectors

	void InverseKinematicsController::updateEndEffectorValues(std::vector<SkeletalEndEffector*>* pEndEffectors) {
		if (nullptr == pEndEffectors) throw NullpointerExcept("pEndEffectors");

		for (auto i : (*pEndEffectors)) {
			Joint* pEff = (i->Segment == HEAD) ? m_pHead : m_JointChains.at(i->Segment).front();
			i->EndEffectorPoints = pEff->pEndEffectorData->GlobalEndEffectorPoints;
			i->TargetPoints = pEff->pEndEffectorData->GlobalTargetPoints;
		}
	}//updateEndEffectorValues

	void InverseKinematicsController::translateTarget(SkeletalSegment SegmentID, Vector3f Translation) {
		EndEffectorData* pEffData = (SegmentID == HEAD) ? m_pHead->pEndEffectorData : m_JointChains.at(SegmentID).front()->pEndEffectorData;
		pEffData->GlobalTargetPoints.colwise() += Translation;
	}//endEffectorTarget

	Eigen::Vector3f InverseKinematicsController::rootPosition(void) {
		return m_pRoot->LocalPosition; // m_pRoot->LocalPosition == m_pRoot->GlobalPosition
	}//rootPosition

	void InverseKinematicsController::rootPosition(Eigen::Vector3f Position) {
		m_pRoot->LocalPosition = Position; // m_pRoot->LocalPosition == m_pRoot->GlobalPosition
	}//rootPosition

}//name space