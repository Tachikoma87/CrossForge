#include "VertexUtility.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	VertexUtility::VertexUtility(void): CForgeObject("VertexUtility") {
		m_PositionOffset = 0;
		m_NormalOffset = 0;
		m_TangentOffset = 0;
		m_UVWOffset = 0;
		m_ColorOffset = 0;
		m_BoneIndicesOffset = 0;
		m_BoneWeightsOffset = 0;

		m_Properties = 0;
		m_VertexSize = 0;
	}//Constructor

	VertexUtility::~VertexUtility(void) {
		clear();
	}//Destructor

	void VertexUtility::init(uint16_t Properties) {
		clear();

		m_Properties = Properties;

		if (hasProperties(VPROP_POSITION)) {
			m_PositionOffset = 0;
			m_VertexSize += sizeof(float) * 3;
		}
		if (hasProperties(VPROP_NORMAL)) {
			m_NormalOffset = m_VertexSize;
			m_VertexSize += sizeof(float) * 3;
		}
		if (hasProperties(VPROP_TANGENT)) {
			m_TangentOffset = m_VertexSize;
			m_VertexSize += sizeof(float) * 3;
		}
		if (hasProperties(VPROP_UVW)) {
			m_UVWOffset = m_VertexSize;
			m_VertexSize += sizeof(float) * 3;
		}
		if (hasProperties(VPROP_COLOR)) {
			m_ColorOffset = m_VertexSize;
			m_VertexSize += sizeof(float) * 3;
		}
		if (hasProperties(VPROP_BONEINDICES)) {
			m_BoneIndicesOffset = m_VertexSize;
			m_VertexSize += sizeof(int32_t) * 4;
		}
		if (hasProperties(VPROP_BONEWEIGHTS)) {
			m_BoneWeightsOffset = m_VertexSize;
			m_VertexSize += sizeof(float) * 4;
		}


	}//initialize

	void VertexUtility::clear(void) {
		m_PositionOffset = 0;
		m_NormalOffset = 0;
		m_TangentOffset = 0;
		m_UVWOffset = 0;
		m_ColorOffset = 0;
		m_BoneIndicesOffset = 0;
		m_BoneWeightsOffset = 0;

		m_Properties = 0;
		m_VertexSize = 0;
	}//clear

	uint16_t VertexUtility::offset(VertexProperty Prop) {
		uint16_t Rval = 0;
		switch (Prop) {
		case VPROP_POSITION: Rval = m_PositionOffset; break;
		case VPROP_NORMAL: Rval = m_NormalOffset; break;
		case VPROP_TANGENT: Rval = m_TangentOffset; break;
		case VPROP_UVW: Rval = m_UVWOffset; break;
		case VPROP_COLOR: Rval = m_ColorOffset; break;
		case VPROP_BONEINDICES: Rval = m_BoneIndicesOffset; break;
		case VPROP_BONEWEIGHTS: Rval = m_BoneWeightsOffset; break;
		default: {
			throw CForgeExcept("Unknown vertex property specified!");
		}break;
		}//switch[vertex property]
		return Rval;
	}//offset

	bool VertexUtility::hasProperties(VertexProperty Props) {
		return (m_Properties & Props);
	}//hasProperty

	uint16_t VertexUtility::vertexSize(void)const {
		return m_VertexSize;
	}//vertexSize

	void buildSkeletonAssignment(const T3DMesh<float>* pMesh, vector<Vector4i>* pBoneIndices, vector<Vector4f>* pSkinningWeights) {
		if (pMesh->boneCount() == 0) return; // nothing to do here
		if (nullptr == pBoneIndices) throw NullpointerExcept("pBoneIndices");
		if (nullptr == pSkinningWeights) throw NullpointerExcept("pSkinningWeights");

		// initialize the arrays
		pBoneIndices->clear();
		pSkinningWeights->clear();

		for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
			pBoneIndices->push_back(Vector4i(-1, -1, -1, -1));
			pSkinningWeights->push_back(Vector4f::Zero());
		}//for[all vertices]


		for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
			const T3DMesh<float>::Bone* pBone = pMesh->getBone(i);

			uint32_t BoneID = pBone->ID;

			for(uint32_t k=0; k < pBone->VertexInfluences.size(); ++k){
				const uint32_t VertexID = pBone->VertexInfluences[k];
				const float VertexWeight = pBone->VertexWeights[k];
				for (uint8_t l = 0; l < 4; ++l) {
					if (pBoneIndices->at(VertexID)[l] < 0) {
						pBoneIndices->at(VertexID)[l] = BoneID;
						pSkinningWeights->at(VertexID)[l] = VertexWeight;
						break;
					}
				}//for[4 values]
			}//for[all vertex influences]	
		}//for[all bones]

		// set all invalid indices to zero (or there may be errors in the shader later)
		for(auto &i: (*pBoneIndices)) {
			for (uint8_t k = 0; k < 4; ++k) {
				if (i[k] < 0) i[k] = 0;
			}//for[4 values of the vector]
		}//For[all bone indices]

	}//buildSkeletonAssignment

	void VertexUtility::buildBuffer(uint32_t VertexCount, void** ppBuffer, uint32_t* pBufferSize, const T3DMesh<float>* pMesh) {
		if (nullptr == ppBuffer) throw NullpointerExcept("ppBuffer");
		if (nullptr == pBufferSize) throw NullpointerExcept("pBufferSize");
		if (0 == VertexCount) throw CForgeExcept("Invalid vertex count for buffer building specified!");

		if (0 == m_VertexSize) throw CForgeExcept("Vertex utility was not initialized. Vertex size is still 0!");

		// compute size and allocate memory
		uint32_t BufferSize = m_VertexSize * VertexCount;
		uint8_t* pBuf = new uint8_t[BufferSize];
		if (nullptr == pBuf) throw OutOfMemoryExcept("pBuf");
		// initialize memory with zeros
		memset(pBuf, 0, BufferSize);

		// assign return values
		(*ppBuffer) = (void*)pBuf;
		(*pBufferSize) = BufferSize;

		if (nullptr == pMesh || 0 == pMesh->vertexCount()) return; // thats it

		std::vector<Vector4i> BoneIndices;
		std::vector<Vector4f> SkinningWeights;

		buildSkeletonAssignment(pMesh, &BoneIndices, &SkinningWeights);

		// initialize buffer with mesh values (if available)	
		for (uint32_t i = 0; i < std::min(pMesh->vertexCount(), VertexCount); ++i) {

			// positions
			uint32_t BufferOffset = i * m_VertexSize + m_PositionOffset;
			float* pV = (float*)&pBuf[BufferOffset];
			pV[0] = pMesh->vertex(i).x();
			pV[1] = pMesh->vertex(i).y();
			pV[2] = pMesh->vertex(i).z();

			// normals
			if (pMesh->normalCount() == pMesh->vertexCount()) {
				BufferOffset = i * m_VertexSize + m_NormalOffset;
				pV = (float*)&pBuf[BufferOffset];
				pV[0] = pMesh->normal(i).x();
				pV[1] = pMesh->normal(i).y();
				pV[2] = pMesh->normal(i).z();
			}

			if (pMesh->tangentCount() == pMesh->vertexCount()) {
				BufferOffset = i * m_VertexSize + m_TangentOffset;
				pV = (float*)&pBuf[BufferOffset];
				pV[0] = pMesh->tangent(i).x();
				pV[1] = pMesh->tangent(i).y();
				pV[2] = pMesh->tangent(i).z();
			}

			if (pMesh->textureCoordinatesCount() == pMesh->vertexCount()) {
				BufferOffset = i * m_VertexSize + m_UVWOffset;
				pV = (float*)&pBuf[BufferOffset];
				pV[0] = pMesh->textureCoordinate(i).x();
				pV[1] = pMesh->textureCoordinate(i).y();
				pV[2] = pMesh->textureCoordinate(i).z();
			}

			if (pMesh->colorCount() == pMesh->vertexCount()) {
				BufferOffset = i * m_VertexSize + m_ColorOffset;
				pV = (float*)&pBuf[BufferOffset];
				pV[0] = pMesh->color(i).x();
				pV[1] = pMesh->color(i).y();
				pV[2] = pMesh->color(i).z();
			}

			if (BoneIndices.size() == pMesh->vertexCount()) {
				BufferOffset = i * m_VertexSize + m_BoneIndicesOffset;
				int32_t *pVi = (int32_t*)&pBuf[BufferOffset];
				pVi[0] = BoneIndices[i].x();
				pVi[1] = BoneIndices[i].y();
				pVi[2] = BoneIndices[i].z();
				pVi[3] = BoneIndices[i].w();
			}

			if (SkinningWeights.size() == pMesh->vertexCount()) {
				BufferOffset = i * m_VertexSize + m_BoneWeightsOffset;
				pV = (float*)&pBuf[BufferOffset];
				pV[0] = SkinningWeights[i].x();
				pV[1] = SkinningWeights[i].y();
				pV[2] = SkinningWeights[i].z();
				pV[3] = SkinningWeights[i].w();
			}

		}//for[number of vertices]	

		BoneIndices.clear();
		SkinningWeights.clear();

	}//buildBuffer

}//name space