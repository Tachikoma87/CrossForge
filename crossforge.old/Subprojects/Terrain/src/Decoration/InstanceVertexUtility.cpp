#include "InstanceVertexUtility.h"
#include <iostream>
using namespace Eigen;
using namespace std;

namespace CForge {

	InstanceVertexUtility::InstanceVertexUtility(void): CForgeObject("VertexUtility") {
		m_PositionOffset = 0;
		m_NormalOffset = 0;
		m_TangentOffset = 0;
		m_UVWOffset = 0;
		m_InstanceMatrixesOffset = 0;

		m_Properties = 0;
		m_VertexSize = 0;
	}//Constructor

	InstanceVertexUtility::~InstanceVertexUtility(void) {
		clear();
	}//Destructor

	void InstanceVertexUtility::init(uint16_t Properties) {
		clear();

		m_Properties = Properties;

		m_PositionOffset = 0;
		m_VertexSize += sizeof(float) * 3;
		
		m_NormalOffset = m_VertexSize;
		m_VertexSize += sizeof(float) * 3;
		
		m_TangentOffset = m_VertexSize;
		m_VertexSize += sizeof(float) * 3;
		
		m_UVWOffset = m_VertexSize;
		m_VertexSize += sizeof(float) * 3;
		
		m_InstanceMatrixesOffset = m_VertexSize;
		


	}//initialize

	void InstanceVertexUtility::clear(void) {
		m_PositionOffset = 0;
		m_NormalOffset = 0;
		m_TangentOffset = 0;
		m_UVWOffset = 0;

		m_InstanceMatrixesOffset = 0;

		m_Properties = 0;
		m_VertexSize = 0;
	}//clear

	uint16_t InstanceVertexUtility::offset(VertexProperty Prop) {
		uint16_t Rval = 0;
		switch (Prop) {
		case VPROP_POSITION: Rval = m_PositionOffset; break;
		case VPROP_NORMAL: Rval = m_NormalOffset; break;
		case VPROP_TANGENT: Rval = m_TangentOffset; break;
		case VPROP_UVW: Rval = m_UVWOffset; break;
		default: {
			throw CForgeExcept("Unknown vertex property specified!");
		}break;
		}//switch[vertex property]
		return Rval;
	}//offset

	bool InstanceVertexUtility::hasProperties(VertexProperty Props) {
		return (m_Properties & Props);
	}//hasProperty

	uint16_t InstanceVertexUtility::vertexSize(void)const {
		return m_VertexSize;
	}//vertexSize

	
	void InstanceVertexUtility::buildBuffer(uint32_t VertexCount, void** ppBuffer, uint32_t* pBufferSize, const T3DMesh<float>* pMesh, std::vector<Matrix4f>& transMatrixes) {
		if (nullptr == ppBuffer) throw NullpointerExcept("ppBuffer");
		if (nullptr == pBufferSize) throw NullpointerExcept("pBufferSize");
		if (0 == VertexCount) throw CForgeExcept("Invalid vertex count for buffer building specified!");

		if (0 == m_VertexSize) throw CForgeExcept("Vertex utility was not initialized. Vertex size is still 0!");

		// compute size and allocate memory
	
		uint32_t BufferSize = m_VertexSize * VertexCount + transMatrixes.size() * sizeof(float) * 4 * 4;
		uint8_t* pBuf = new uint8_t[BufferSize];
		if (nullptr == pBuf) throw OutOfMemoryExcept("pBuf");
		// initialize memory with zeros
		memset(pBuf, 0, BufferSize);

		// assign return values
		(*ppBuffer) = (void*)pBuf;
		(*pBufferSize) = BufferSize;

		if (nullptr == pMesh || 0 == pMesh->vertexCount()) return; // thats it

		// initialize buffer with mesh values (if available)	
		uint32_t BufferOffset;
		for (uint32_t i = 0; i < std::min(pMesh->vertexCount(), VertexCount); ++i) {

			// positions
			BufferOffset = i * m_VertexSize + m_PositionOffset;
			float* pV = (float*)&pBuf[BufferOffset];
			pV[0] = pMesh->vertex(i).x();
			pV[1] = pMesh->vertex(i).y();
			pV[2] = pMesh->vertex(i).z();

		
			BufferOffset = i * m_VertexSize + m_NormalOffset;
			pV = (float*)&pBuf[BufferOffset];
			pV[0] = pMesh->normal(i).x();
			pV[1] = pMesh->normal(i).y();
			pV[2] = pMesh->normal(i).z();
			

			
			BufferOffset = i * m_VertexSize + m_TangentOffset;
			pV = (float*)&pBuf[BufferOffset];
			pV[0] = pMesh->tangent(i).x();
			pV[1] = pMesh->tangent(i).y();
			pV[2] = pMesh->tangent(i).z();
			

			
			BufferOffset = i * m_VertexSize + m_UVWOffset;
			pV = (float*)&pBuf[BufferOffset];
			pV[0] = pMesh->textureCoordinate(i).x();
			pV[1] = pMesh->textureCoordinate(i).y();
			pV[2] = pMesh->textureCoordinate(i).z();
					

		}//for[number of vertices]	
		BufferOffset = std::min(pMesh->vertexCount(), VertexCount) * m_VertexSize;

		float* pV = (float*)&pBuf[BufferOffset];
		for (int i = 0; i < 4 * 4 * transMatrixes.size(); i++) {
			pV[i] = transMatrixes.data()->data()[i];
			
		}

	}//buildBuffer

}//name space