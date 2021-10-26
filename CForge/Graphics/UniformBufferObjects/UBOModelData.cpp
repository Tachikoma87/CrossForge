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
	
		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_DYNAMIC_DRAW, nullptr, size());
		m_ModelMatrixOffset = 0;
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

		return Rval;
	}//size

	void UBOModelData::modelMatrix(Eigen::Matrix4f Mat) {
		m_Buffer.bufferSubData(m_ModelMatrixOffset, 16 * sizeof(float), Mat.data());
	}//modelMatrix

}//name space