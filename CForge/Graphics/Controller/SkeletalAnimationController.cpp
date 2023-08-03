#include "SkeletalAnimationController.h"
#include "../Shader/SShaderManager.h"
#include "../../Math/CForgeMath.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	SkeletalAnimationController::SkeletalAnimationController(void): CForgeObject("SkeletalAnimationController") {
		m_pRoot = nullptr;
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

	SkeletalAnimationController::~SkeletalAnimationController(void) {
		clear();
	}//Destructor

	// pMesh has to hold skeletal definition
	void SkeletalAnimationController::init(T3DMesh<float>* pMesh, bool CopyAnimationData) {
		clear();

		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->boneCount() == 0) throw CForgeExcept("Mesh has no bones!");


		// create bones and copy data
		for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
			const T3DMesh<float>::Bone* pRef = pMesh->getBone(i);
		
			Joint* pJoint = new Joint();
			pJoint->ID = pRef->ID;
			pJoint->Name = pRef->Name;
			pJoint->LocalPosition = Vector3f::Zero();
			pJoint->LocalRotation = Quaternionf::Identity();
			pJoint->LocalScale = Vector3f::Ones();
			pJoint->OffsetMatrix = pRef->OffsetMatrix;
			pJoint->SkinningMatrix = Matrix4f::Identity();

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

		if (CopyAnimationData) {
			for (uint32_t i = 0; i < pMesh->skeletalAnimationCount(); ++i) {
				addAnimationData(pMesh->getSkeletalAnimation(i));
			}//for[skeletalAnimationData]
		}//if[copy animation data]

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

	void SkeletalAnimationController::clear(void) {
		m_pRoot = nullptr;
		for (auto& i : m_Joints) if (nullptr != i) delete i;
		for (auto& i : m_SkeletalAnimations) if (nullptr != i) delete i;
		for (auto& i : m_ActiveAnimations) if (nullptr != i) delete i;
		m_Joints.clear();
		m_SkeletalAnimations.clear();
		m_ActiveAnimations.clear();

		m_UBO.clear();

		// instances get deleted by the Shader Manager
		m_pShadowPassShader = nullptr;
		m_pShadowPassFSCode = nullptr;
		m_pShadowPassVSCode = nullptr;

	}//clear

	GLShader* SkeletalAnimationController::shadowPassShader(void) {
		return m_pShadowPassShader;
	}//shadowPassShader

	void SkeletalAnimationController::addAnimationData(T3DMesh<float>::SkeletalAnimation* pAnimation) {
		if (nullptr == pAnimation) throw NullpointerExcept("pAnimation");

		
		T3DMesh<float>::SkeletalAnimation* pAnim = new T3DMesh<float>::SkeletalAnimation();
		pAnim->Duration = pAnimation->Duration;
		pAnim->Name = pAnimation->Name;
		pAnim->Speed = pAnimation->Speed;

		// keyframes and join names have to match
		for (uint32_t i = 0; i < pAnimation->Keyframes.size(); ++i) {
			int32_t JointID = jointIDFromName(pAnimation->Keyframes[i]->BoneName);
			if (JointID < 0) continue;
			while (pAnim->Keyframes.size() <= JointID) pAnim->Keyframes.push_back(new T3DMesh<float>::BoneKeyframes());
			(*pAnim->Keyframes[JointID]) = (*pAnimation->Keyframes[i]);
			pAnim->Keyframes[JointID]->BoneID = JointID;
		}

		m_SkeletalAnimations.push_back(pAnim);
		
		int32_t KeyframeMaxTimestamps = 0;
		int32_t MaxTimestamps = 0;

		for (uint32_t i = 0; i < pAnim->Keyframes.size(); ++i) {
			if (pAnim->Keyframes[i]->Timestamps.size() > MaxTimestamps) {
				MaxTimestamps = pAnim->Keyframes[i]->Timestamps.size();
				KeyframeMaxTimestamps = i;
			}
		}


		for (uint32_t i = 0; i < pAnim->Keyframes.size(); ++i) {
			auto* pKeyFrame = pAnim->Keyframes[i];
			if (pKeyFrame->BoneName.empty()) continue;
			if (pKeyFrame->Timestamps.size() != MaxTimestamps) pKeyFrame->Timestamps = pAnim->Keyframes[KeyframeMaxTimestamps]->Timestamps;
			Vector3f Pos = pKeyFrame->Positions[0];
			Vector3f Scale = pKeyFrame->Scalings[0];
			Quaternionf Rot = pKeyFrame->Rotations[0];


			while (pKeyFrame->Positions.size() < MaxTimestamps) pKeyFrame->Positions.push_back(Pos);
			while (pKeyFrame->Scalings.size() < MaxTimestamps) pKeyFrame->Scalings.push_back(Scale);
			while (pKeyFrame->Rotations.size() < MaxTimestamps) pKeyFrame->Rotations.push_back(Rot);
		}
	
	}//addAnimation

	int32_t SkeletalAnimationController::jointIDFromName(std::string JointName) {
		int32_t Rval = -1;

		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			if (m_Joints[i]->Name.compare(JointName) == 0) Rval = i;
		}

		return Rval;
	}//jointIDFromName

	SkeletalAnimationController::Animation* SkeletalAnimationController::createAnimation(int32_t AnimationID, float Speed, float Offset) {
		Animation* pRval = new Animation();
		pRval->AnimationID = AnimationID;
		pRval->Speed = Speed;
		pRval->t = Offset;
		pRval->Finished = false;
		pRval->Duration = m_SkeletalAnimations[AnimationID]->Duration;
		Animation* pTemp = pRval;
		for (uint32_t i = 0; i < m_ActiveAnimations.size(); ++i) {
			if (m_ActiveAnimations[i] == nullptr) {
				m_ActiveAnimations[i] = pTemp;
				pTemp = nullptr;
				break;
			}
		}

		if(nullptr != pTemp) m_ActiveAnimations.push_back(pTemp);

		return pRval;
	}//createAnimation

	void SkeletalAnimationController::update(float FPSScale) {
		for (auto i : m_ActiveAnimations) {
			if (nullptr != i && !i->Finished) {
				i->t += FPSScale * i->Speed;
			}
		}//for[active animations]
	}//update

	//Addition for pausing and resuming animations
	void SkeletalAnimationController::pauseAnimation(Animation* pAnim) {
		if (pAnim != nullptr) {
			pAnim->Speed = 0.0f; // Set the animation speed to 0 to pause it
		}
	}

	void SkeletalAnimationController::resumeAnimation(Animation* pAnim) {
		if (pAnim != nullptr) {
			pAnim->Speed = 1.0f; // Set the animation speed back to 1 to resume it
		}
	}

	void SkeletalAnimationController::destroyAnimation(Animation* pAnim) {

		for (auto& i : m_ActiveAnimations) {
			if (i != nullptr && i == pAnim) {
				delete i;
				i = nullptr;
				break;
			}
		}

	}//destroyAnimation

	void SkeletalAnimationController::applyAnimation(Animation* pAnim, bool UpdateUBO) {

		if (nullptr == pAnim) {
			for (auto i : m_Joints) i->SkinningMatrix = Eigen::Matrix4f::Identity();
		}
		else {
			T3DMesh<float>::SkeletalAnimation* pAnimData = m_SkeletalAnimations[pAnim->AnimationID];

			/*if (pAnim->t > pAnimData->Duration) {
				pAnim->t = pAnimData->Duration;
				pAnim->Finished = true;
			} */
			if (pAnim != nullptr && pAnim->t >= pAnimData->Duration) {
				pAnim->t = pAnimData->Duration; 
				pAnim->Finished = true;
			}

			// apply local transformations
			for (uint32_t i = 0; i < pAnimData->Keyframes.size(); ++i) {

				if (pAnimData->Keyframes[i]->BoneName.empty()) continue;

				if (pAnimData->Keyframes[i]->Timestamps.size() == 0) continue;

				for (uint32_t k = 0; k < pAnimData->Keyframes[i]->Timestamps.size() - 1; ++k) {
					float Time = pAnimData->Keyframes[i]->Timestamps[k];
					float TimeP1 = pAnimData->Keyframes[i]->Timestamps[k + 1];

					if (Time <= pAnim->t && TimeP1 > pAnim->t) {

						float s = (pAnim->t - Time) / (TimeP1 - Time);
						if (i < m_Joints.size()) {
							m_Joints[i]->LocalPosition = (1.0f - s) * pAnimData->Keyframes[i]->Positions[k] + s * pAnimData->Keyframes[i]->Positions[k + 1];
							m_Joints[i]->LocalRotation = pAnimData->Keyframes[i]->Rotations[k].slerp(s, pAnimData->Keyframes[i]->Rotations[k + 1]);
							m_Joints[i]->LocalScale = (1.0f - s) * pAnimData->Keyframes[i]->Scalings[k] + s * pAnimData->Keyframes[i]->Scalings[k + 1];
						}		
						break;
					}
				}

			}//for[keyframes]

			transformSkeleton(m_pRoot, Matrix4f::Identity());
		}

		if (UpdateUBO) {
			for (uint32_t i = 0; i < m_Joints.size(); ++i) m_UBO.skinningMatrix(i, m_Joints[i]->SkinningMatrix);
		}
		
	}//applyAnimation

	UBOBoneData* SkeletalAnimationController::ubo(void) {
		return &m_UBO;
	}//ubo


	void SkeletalAnimationController::transformSkeleton(Joint* pJoint, Eigen::Matrix4f ParentTransform) {
		
		const Matrix4f R = CForgeMath::rotationMatrix(pJoint->LocalRotation);
		const Matrix4f T = CForgeMath::translationMatrix(pJoint->LocalPosition);
		const Matrix4f S = CForgeMath::scaleMatrix(pJoint->LocalScale);
		const Matrix4f JointTransform = T * R * S;

		Matrix4f LocalTransform =  ParentTransform * JointTransform;
		pJoint->SkinningMatrix = LocalTransform * pJoint->OffsetMatrix;

		for (auto i : pJoint->Children) transformSkeleton(i, LocalTransform);
	}//transformSkeleton

	T3DMesh<float>::SkeletalAnimation* SkeletalAnimationController::animation(uint32_t ID) {
		if (ID >= m_SkeletalAnimations.size()) throw IndexOutOfBoundsExcept("ID");
		return m_SkeletalAnimations[ID];
	}//animation

	uint32_t SkeletalAnimationController::animationCount(void)const {
		return m_SkeletalAnimations.size();
	}//animationCount

	UBOBoneData* SkeletalAnimationController::boneUBO(void) {
		return &m_UBO;
	}

	void SkeletalAnimationController::retrieveSkinningMatrices(std::vector<Eigen::Matrix4f>* pSkinningMats) {
		if (nullptr == pSkinningMats) throw NullpointerExcept("pSkinningMats");
		pSkinningMats->clear();
		for (auto i : m_Joints) pSkinningMats->push_back(i->SkinningMatrix);
	}//retrieveSkinningMatrices

	std::vector<SkeletalAnimationController::SkeletalJoint*> SkeletalAnimationController::retrieveSkeleton(void)const {
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

	void SkeletalAnimationController::updateSkeletonValues(std::vector<SkeletalAnimationController::SkeletalJoint*>* pSkeleton) {
		if (nullptr == pSkeleton) throw NullpointerExcept("pSkeleton");

		for (auto i : (*pSkeleton)) {
			i->OffsetMatrix = m_Joints[i->ID]->OffsetMatrix;
			i->LocalPosition = m_Joints[i->ID]->LocalPosition;
			i->LocalRotation = m_Joints[i->ID]->LocalRotation;
			i->LocalScale = m_Joints[i->ID]->LocalScale;
			i->SkinningMatrix = m_Joints[i->ID]->SkinningMatrix;
		}
	}//updateSkeleton

}//name space