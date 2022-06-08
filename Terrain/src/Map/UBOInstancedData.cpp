#include "UBOInstancedData.h"


namespace CForge {

	UBOInstancedData::UBOInstancedData(void) : CForgeObject("UBOInstancedData") {
		m_instanceCount = 0;
	}//Constructor

	UBOInstancedData::~UBOInstancedData(void) {
		clear();
	}//Destructor

	void UBOInstancedData::init() {
		clear();
		//m_instanceCount = instanceCount;
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

	void UBOInstancedData::setInstance(const Eigen::Matrix4f* mat, uint32_t index)
	{
		float dat[16];
		const float* pMat = mat->data();
		for (uint32_t j = 0; j < 16; j++) {
			dat[j] = pMat[j];
		}

		//m_Buffer.bufferSubData(i * 12 * sizeof(float), 12 * sizeof(float), dat);
		m_Buffer.bufferSubData(index * 16 * sizeof(float), 16 * sizeof(float), dat);
	}

	void UBOInstancedData::setInstances(const std::vector<Eigen::Matrix4f>* mats, Eigen::Vector2i range)
	{
		m_instanceCount = range.y() - range.x();

		for (uint32_t i = 0; i < m_instanceCount; i++) {
			if (i >= m_maxInstanceCount)
				break;

			float dat[16];
			const float* pMat = mats->at(i + range.x()).data();
			for (uint32_t j = 0; j < 16; j++) {
				dat[j] = pMat[j];
			}

			//m_Buffer.bufferSubData(i * 12 * sizeof(float), 12 * sizeof(float), dat);
			m_Buffer.bufferSubData(i * 16 * sizeof(float), 16 * sizeof(float), dat);
		}
	}

	uint32_t UBOInstancedData::getMaxInstanceCount() {
		return m_maxInstanceCount;
	}

}//name space