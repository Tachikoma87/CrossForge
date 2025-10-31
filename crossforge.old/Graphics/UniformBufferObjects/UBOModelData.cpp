#include "UBOModelData.h"


namespace CForge {

	UBOModelData::UBOModelData(void): CForgeObject("UBOModelData") {
		m_ModelMatrixOffset = 0;
	}//Constructor

	UBOModelData::~UBOModelData(void) {
		clear();
	}//Destructor

	void UBOModelData::init(void) {
		clear();
	
		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_STATIC_DRAW, nullptr, size());
		m_ModelMatrixOffset = 0;
		m_NormalMatrixOffset = 16 * sizeof(float);
	}//initialize

	void UBOModelData::clear(void) {
		m_Buffer.clear();
		m_ModelMatrixOffset = 0;
	}//clear


	void UBOModelData::bind(uint32_t BindingPoint) {
		m_Buffer.bindBufferBase(BindingPoint);
	}//bind

	uint32_t UBOModelData::size(void)const {
		uint32_t Rval = 0;

		Rval += 16 * sizeof(float); // Model matrix
		Rval += 12 * sizeof(float); // Normal Matrix

		return Rval;
	}//size

	void UBOModelData::modelMatrix(Eigen::Matrix4f Mat) {
		m_Buffer.bufferSubData(m_ModelMatrixOffset, 16 * sizeof(float), Mat.data());

	}//modelMatrix

	void UBOModelData::normalMatrix(Eigen::Matrix4f Mat) {
		m_Buffer.bufferSubData(m_NormalMatrixOffset, 12 * sizeof(float), Mat.block<3,4>(0,0).data());
	}//normalMatrix

}//name space