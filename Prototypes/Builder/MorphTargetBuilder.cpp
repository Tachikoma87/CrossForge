#include "MorphTargetBuilder.h"

namespace CForge {

	MorphTargetBuilder::MorphTargetBuilder(void) {

	}//Constructor

	MorphTargetBuilder::~MorphTargetBuilder(void) {
		clear();
	}//Destructor

	void MorphTargetBuilder::init(T3DMesh<float>* pBaseMesh) {
		if (nullptr == pBaseMesh) throw NullpointerExcept("pBaseMesh");
		if (pBaseMesh->vertexCount() == 0) throw CForgeExcept("Specified mesh contains no vertex data and is therefore unsuitable for morph target building!");
		clear();

		m_BaseMesh.init(pBaseMesh);

	}//initialize

	void MorphTargetBuilder::clear(void) {
		m_BaseMesh.clear();
		for (auto& i : m_MorphTargets) delete i;
		for (auto& i : m_Targets) delete i;
		m_MorphTargets.clear();
		m_Targets.clear();
	}//clear

	void MorphTargetBuilder::addTarget(T3DMesh<float>* pTarget, std::string Name) {
		if (nullptr == pTarget) throw NullpointerExcept("pTarget");
		if (pTarget->vertexCount() != m_BaseMesh.vertexCount()) throw CForgeExcept("Number of mesh vertices differs from base mesh vertex count. Mesh is unsuited to be a morph target!");

		Target* pNewTarget = new Target();
		pNewTarget->Name = Name;
		for (uint32_t i = 0; i < pTarget->vertexCount(); ++i) pNewTarget->Positions.push_back(pTarget->vertex(i));
		for (uint32_t i = 0; i < pTarget->normalCount(); ++i) pNewTarget->Normals.push_back(pTarget->normal(i));
		m_Targets.push_back(pNewTarget);
	}//addTarget

	void MorphTargetBuilder::build(void) {
		for (auto& i : m_MorphTargets) delete i;
		m_MorphTargets.clear();

		for (auto i : m_Targets) {
			T3DMesh<float>::MorphTarget* pMT = new T3DMesh<float>::MorphTarget();

			for (uint32_t k = 0; k < m_BaseMesh.vertexCount(); ++k) {
				Eigen::Vector3f Offset = i->Positions[k] - m_BaseMesh.vertex(k);
				pMT->VertexIDs.push_back(k);
				pMT->VertexOffsets.push_back(Offset);

				if (m_BaseMesh.normalCount() > k && i->Normals.size() >= k) {
					Offset = i->Normals[k] - m_BaseMesh.normal(k);
					pMT->NormalOffsets.push_back(Offset);
				}
			}//for[all vertices (and normals)]

			pMT->ID = m_MorphTargets.size();
			pMT->Name = i->Name;

			m_MorphTargets.push_back(pMT);
		}//for[all targets]
	}//build

	void MorphTargetBuilder::retrieveMorphTargets(T3DMesh<float>* pMesh) {
		for (auto i : m_MorphTargets) {
			pMesh->addMorphTarget(i, true);
		}//for[all morph targets]
	}//retrieveMorphTargets

}//name space