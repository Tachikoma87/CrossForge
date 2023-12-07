#include "MorphTargetAnimationController.h"
#include "../../Math/CForgeMath.h"
#include "../../Utility/CForgeUtility.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	MorphTargetAnimationController::MorphTargetAnimationController(void): CForgeObject("MorphTargetAnimationController") {

	}//Constructor

	MorphTargetAnimationController::~MorphTargetAnimationController(void) {
		clear();
	}//Destructor

	void MorphTargetAnimationController::init(T3DMesh<float>* pMesh) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->morphTargetCount() == 0) throw CForgeExcept("Mesh contains no morph targets. Can not create morph target animation controller!");
		clear();

		// create a morph target from every defined morph target
		for (uint32_t i = 0; i < pMesh->morphTargetCount(); ++i) {
			T3DMesh<float>::MorphTarget* pMT = pMesh->getMorphTarget(i);

			MorphTarget* pNewMT = new MorphTarget();
			pNewMT->ID = i;
			pNewMT->Name = pMT->Name;
			m_MorphTargets.push_back(pNewMT);

		}//for[all morph targets]

	}//initialize

	void MorphTargetAnimationController::clear(void) {
		for (auto& i : m_ActiveAnimations) {
			delete i;
			i = nullptr;
		}
		for (auto& i : m_MorphTargets) {
			delete i;
			i = nullptr;
		}
		for (auto& i : m_AnimationSequences) {
			delete i;
			i = nullptr;
		}
		m_ActiveAnimations.clear();
		m_MorphTargets.clear();
		m_AnimationSequences.clear();
	}//clear


	void MorphTargetAnimationController::update(float FPSScale) {
		for (auto i : m_ActiveAnimations) progress(i, FPSScale);
	}//update

	int32_t MorphTargetAnimationController::addAnimationSequence(AnimationSequence* pSequence) {
		if (nullptr == pSequence) return -1;

		AnimationSequence* pSeq = new AnimationSequence();
		pSeq->ID = -1;
		pSeq->Name = pSequence->Name;
		pSeq->Parameters = pSequence->Parameters;
		pSeq->Targets = pSequence->Targets;

		for (size_t i = 0; i < m_AnimationSequences.size(); ++i) {
			if (nullptr == m_AnimationSequences[i]) {
				pSeq->ID = i;
				break;
			}
		}
		if (pSeq->ID == -1) {
			pSeq->ID = m_AnimationSequences.size();
			m_AnimationSequences.push_back(nullptr);
		}

		m_AnimationSequences[pSeq->ID] = pSeq;
		return pSeq->ID;
	}//addAnimationSequence

	MorphTargetAnimationController::ActiveAnimation* MorphTargetAnimationController::play(int32_t SequenceID, float Speed) {
		if (SequenceID < 0 || SequenceID >= int32_t(m_AnimationSequences.size())) throw IndexOutOfBoundsExcept("SequenceID");

		ActiveAnimation* pRval = new ActiveAnimation();
		pRval->SequenceID = SequenceID;
		pRval->Speed = Speed;
		pRval->t = 0.0f;
		pRval->CurrentSquenceIndex = 0;
		pRval->SequenceStartTimestamp = CForgeUtility::timestamp();
		pRval->Finished = false;

		pRval->ID = -1;
		for (size_t i = 0; i < m_ActiveAnimations.size(); ++i) {
			if (nullptr == m_ActiveAnimations[i]) {
				pRval->ID = i;
				break;
			}
		}//for[active animations]

		if (-1 == pRval->ID) {
			pRval->ID = m_ActiveAnimations.size();
			m_ActiveAnimations.push_back(nullptr);
		}

		m_ActiveAnimations[pRval->ID] = pRval;

		return pRval;
	}//play

	void MorphTargetAnimationController::progress(ActiveAnimation* pAnim, float Scale) {
		if (nullptr == pAnim) return;
		if (!valid(pAnim)) return;
		if (pAnim->Finished) return;
		
		AnimationSequence* pSeq = m_AnimationSequences[pAnim->SequenceID];
		float ElapsedTime = float(CForgeUtility::timestamp() - pAnim->SequenceStartTimestamp)/1000.0f;
		Vector3f Params = pSeq->Parameters[pAnim->CurrentSquenceIndex];
		pAnim->t = pAnim->Speed * ElapsedTime/Params[2];

		// progress to next morph target in sequence?
		if (pAnim->t > 1.0f) {
			pAnim->CurrentSquenceIndex++;
			pAnim->SequenceStartTimestamp = CForgeUtility::timestamp();
			pAnim->t = 0.0f;

			// end of sequence reached?
			if (pAnim->CurrentSquenceIndex >= pSeq->Targets.size()) {
				pAnim->Finished = true;
			}
		}
		
	}//progress

	void MorphTargetAnimationController::apply(std::vector<ActiveAnimation*>* pAnims, UBOMorphTargetData* pUBO) {
		if (pAnims == nullptr) return;
		if (pUBO == nullptr) throw NullpointerExcept("pUBO");


		uint32_t ActiveMorphTargets = 0;
		for (auto i : (*pAnims)) {
			if (nullptr == i) continue;
			if (!valid(i)) continue;
			if (i->Finished) continue;

			// retrieve current morph target
			const int32_t MTID = m_AnimationSequences[i->SequenceID]->Targets[i->CurrentSquenceIndex];
			const Vector3f Params = m_AnimationSequences[i->SequenceID]->Parameters[i->CurrentSquenceIndex];
			const float Strength = (1.0f - i->t) * Params[0] + i->t * Params[1];
			pUBO->setMorphTargetParam(ActiveMorphTargets, MTID, Strength);
			ActiveMorphTargets++;

			// 12 is hard coded maximum in shader
			// if we set more than 12, shader will access invalid data23
			if (ActiveMorphTargets >= 12) break; 
		}//for[active animations

		pUBO->activeMorphTargets(ActiveMorphTargets);
	}//apply

	bool MorphTargetAnimationController::valid(ActiveAnimation* pAnim) {	
		if (nullptr == pAnim) return false;
		bool Rval = true;
		if (pAnim->ID < 0 || pAnim->ID >= m_ActiveAnimations.size()) Rval = false;
		else if (m_ActiveAnimations[pAnim->ID] == nullptr) Rval = false;
		return Rval;
	}//valid

	uint32_t MorphTargetAnimationController::morphTargetCount(void)const {
		return m_MorphTargets.size();
	}//morphTargetCount

	MorphTargetAnimationController::MorphTarget* MorphTargetAnimationController::morphTarget(int32_t ID) {
		if (ID < 0 || ID >= m_MorphTargets.size()) throw IndexOutOfBoundsExcept("ID");
		return m_MorphTargets[ID];
	}//morphTarget

	uint32_t MorphTargetAnimationController::animationSequenceCount(void)const {
		return m_AnimationSequences.size();
	}//animationSequenceCount

	MorphTargetAnimationController::AnimationSequence* MorphTargetAnimationController::animationSequence(int32_t ID) {
		if (ID < 0 || ID >= m_AnimationSequences.size()) throw IndexOutOfBoundsExcept("ID");
		return m_AnimationSequences[ID];
	}//animationSequence

}//name space