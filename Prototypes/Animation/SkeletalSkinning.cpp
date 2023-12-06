#include "SkeletalSkinning.h"

namespace CForge {

	SkeletalSkinning::SkeletalSkinning(void) {
		m_pController = nullptr;
	}//

	SkeletalSkinning::~SkeletalSkinning(void) {

	}//Destructor

	void SkeletalSkinning::init(T3DMesh<float>* pMesh, SkeletalAnimationController* pController) {
		for (auto& i : m_SkinVertexes) {
			if (nullptr != i) delete i;
		}
		m_SkinVertexes.clear();

		// create vertexes
		for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
			SkinVertex* pSV = new SkinVertex();
			pSV->V = pMesh->vertex(i);
			m_SkinVertexes.push_back(pSV);
		}//for[vertices]

		// go through bones and store influences/weights
		for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
			auto* pBone = pMesh->getBone(i);

			for (uint32_t k = 0; k < pBone->VertexInfluences.size(); ++k) {
				auto* pAffectedSF = m_SkinVertexes[pBone->VertexInfluences[k]];
				pAffectedSF->BoneInfluences.push_back(i);
				pAffectedSF->BoneWeights.push_back(pBone->VertexWeights[k]);
			}//for[vertex influences]
		}//for[all bones]

		// make sure we have at least 4 influences per vertex
		for (auto i : m_SkinVertexes) {
			while (i->BoneInfluences.size() < 4) {
				i->BoneInfluences.push_back(0);
				i->BoneWeights.push_back(0.0f);
			}
		}//for[skin vertexes]

		m_pController = pController;
	}//initialize

	Eigen::Vector3f SkeletalSkinning::transformVertex(int32_t Index) {

		auto* pV = m_SkinVertexes[Index];

		Eigen::Vector4i I = Eigen::Vector4i(pV->BoneInfluences[0], pV->BoneInfluences[1], pV->BoneInfluences[2], pV->BoneInfluences[3]);
		Eigen::Vector4f W = Eigen::Vector4f(pV->BoneWeights[0], pV->BoneWeights[1], pV->BoneWeights[2], pV->BoneWeights[3]);

		return m_pController->transformVertex(m_SkinVertexes[Index]->V, I, W);
	}//trnasformVertex

}//name space