#include "UBOInstancedData.h"


namespace CForge {

	UBOInstancedData::UBOInstancedData(void) : CForgeObject("UBOInstancedData") {
		m_instanceCount = 0;
	}//Constructor

	UBOInstancedData::~UBOInstancedData(void) {
		clear();
	}//Destructor

	void UBOInstancedData::init(uint32_t instanceCount) {
		clear();
		m_instanceCount = instanceCount;
		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_DYNAMIC_DRAW, nullptr, size());
	}//initialize

	// TODO entfernen, funktion brauche ich nicht, wenn weniger gebraucht, so bleiben sowieso ungenutzt im draw call, wenn mehr dann sowieso neuinitialisieren
	//void UBOInstancedData::setInstanceCount(uint32_t instanceCount) {
	//	
	//	if (instanceCount < m_instanceCount) { // we have less instances than needed
	//		
	//	}
	//	else if (instanceCount > m_instanceCount) { // not enough space, init again
	//		init(instanceCount);
	//	}
	//	m_instanceCount = instanceCount;
	//}

	void UBOInstancedData::clear(void) {
		m_Buffer.clear();
	}//clear

	void UBOInstancedData::bind(uint32_t BindingPoint) {
		m_Buffer.bindBufferBase(BindingPoint);
	}//bind

	uint32_t UBOInstancedData::size(void)const {
		uint32_t Rval = 0;
		Rval += 12 * sizeof(float) * m_instanceCount; // rotation matrix + translation matrix
		return Rval;
	}//size

	void UBOInstancedData::setInstance(uint32_t index, Eigen::Matrix3f rotation, Eigen::Vector3f translation)
	{
		if (index >= m_instanceCount)
			return;
		
		float dat[12];
		float* pRot = rotation.data();		// is it faster like this?
		float* pTrans = translation.data();	
		for (uint32_t i = 0; i < 9; i++) {
			dat[i] = pRot[i];
		}
		for (uint32_t i = 0; i < 3; i++) {
			dat[i+9] = pTrans[i];
		}
		m_Buffer.bufferSubData(index*12*sizeof(float), 12*sizeof(float), dat);
	}

	void UBOInstancedData::setInstance(uint32_t index, Eigen::Matrix4f mat)
	{
		if (index >= m_instanceCount)
			return;
		
		float* pMat = mat.data();
		float dat[12];
		for (uint32_t i = 0; i < 9; i++) {
			dat[i] = pMat[i%3 + (i/3)*4];
		}
		for (uint32_t i = 0; i < 3; i++) {
			dat[i+9] = pMat[12+i];
		}

		m_Buffer.bufferSubData(index * 12 * sizeof(float), 12 * sizeof(float), dat);
	}

}//name space