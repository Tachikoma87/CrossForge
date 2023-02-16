#include "UBOInstancedData.h"
#include <crossforge/Graphics/OpenGLHeader.h>

namespace CForge {

	UBOInstancedData::UBOInstancedData(void) : CForgeObject("UBOInstancedData") {
		m_instanceCount = 0;
	}//Constructor

	UBOInstancedData::~UBOInstancedData(void) {
		clear();
	}//Destructor

	void UBOInstancedData::init() {
		clear();
		GLint max_block_size;
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_block_size);
		//printf("max block: %d \n", max_block_size);
		m_maxInstanceCount = (max_block_size*8)/32/16;
		//printf("instance: %d \n", m_maxInstanceCount);
		
		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_DYNAMIC_DRAW, nullptr, size());
	}//initialize

	void UBOInstancedData::clear(void) {
		m_Buffer.clear();
	}//clear

	void UBOInstancedData::bind(uint32_t BindingPoint) {
		m_Buffer.bindBufferBase(BindingPoint);
	}//bind

	uint32_t UBOInstancedData::size(void)const {
		uint32_t Rval = 0;
		Rval += 16 * sizeof(float) * m_maxInstanceCount; // rotation matrix + translation matrix
		return Rval;
	}//size

	//void UBOInstancedData::setInstance(uint32_t index, Eigen::Matrix3f rotation, Eigen::Vector3f translation)
	//{
	//	if (index >= m_instanceCount)
	//		return;
	//	
	//	float dat[12];
	//	float* pRot = rotation.data();		// is it faster like this?
	//	float* pTrans = translation.data();	
	//	for (uint32_t i = 0; i < 9; i++) {
	//		dat[i] = pRot[i];
	//	}
	//	for (uint32_t i = 0; i < 3; i++) {
	//		dat[i+9] = pTrans[i];
	//	}
	//	m_Buffer.bufferSubData(index*12*sizeof(float), 12*sizeof(float), dat);
	//}

	//void UBOInstancedData::setInstance(uint32_t index, Eigen::Matrix4f mat)
	//{
	//	if (index >= m_instanceCount)
	//		return;
	//	
	//	float* pMat = mat.data();
	//	float dat[12];
	//	for (uint32_t i = 0; i < 9; i++) {
	//		dat[i] = pMat[i%3 + (i/3)*4];
	//	}
	//	for (uint32_t i = 0; i < 3; i++) {
	//		dat[i+9] = pMat[12+i];
	//	}

	//	m_Buffer.bufferSubData(index * 12 * sizeof(float), 12 * sizeof(float), dat);
	//}

	void UBOInstancedData::setInstances(const std::vector<Eigen::Matrix4f>* mats, Eigen::Vector2i range)
	{
		m_instanceCount = range.y()-range.x();
		
		for (uint32_t i = 0; i < m_instanceCount; i++) {
			if (i >= m_maxInstanceCount)
				break;
			//const float* pMat = mats->at(i).data();
			//float dat[12];
			//for (uint32_t i = 0; i < 9; i++) {
			//	dat[i] = pMat[i % 3 + (i / 3) * 4];
			//}
			//for (uint32_t i = 0; i < 3; i++) {
			//	dat[i + 9] = pMat[12 + i];
			//}
			
			float dat[16];
			const float* pMat = mats->at(i+range.x()).data();
			for (uint32_t j = 0; j < 16; j++) {
				dat[j] = pMat[j];
			}
			
			//m_Buffer.bufferSubData(i * 12 * sizeof(float), 12 * sizeof(float), dat);
			m_Buffer.bufferSubData(i * 16 * sizeof(float), 16 * sizeof(float), dat);
		}
	}
	

	void UBOInstancedData::setInstance(const Eigen::Matrix4f* mat, uint32_t index)
	{
		float dat[16];
		const float* pMat = mat->data();
		for (uint32_t j = 0; j < 16; j++) {
			dat[j] = pMat[j];
		}

		m_Buffer.bufferSubData(index * 16 * sizeof(float), 16 * sizeof(float), dat);
	}
	
	uint32_t UBOInstancedData::getMaxInstanceCount() {
		return m_maxInstanceCount;
	}

}//name space