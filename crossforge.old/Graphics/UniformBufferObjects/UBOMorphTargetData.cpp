#include "UBOMorphTargetData.h"

namespace CForge {

	UBOMorphTargetData::UBOMorphTargetData(void): CForgeObject("UBOMorphTargetData") {

	}//Constructor

	UBOMorphTargetData::~UBOMorphTargetData(void) {
		clear();
	}//Destructor

	void UBOMorphTargetData::init(void) {
		clear();

		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_DYNAMIC_DRAW, nullptr, size());
		m_DataOffset = 0;
		m_ActivationIDsOffset = 4 * sizeof(int32_t);
		m_ActivationStrengthsOffset = m_ActivationIDsOffset + 3 * 4 * sizeof(int32_t);

	}//initialize

	void UBOMorphTargetData::clear(void) {
		m_ActivationIDsOffset = 0;
		m_ActivationStrengthsOffset = 0;
		m_DataOffset = 0;
		m_Buffer.clear();
	}//clear


	void UBOMorphTargetData::bind(uint32_t BindingPoint) {
		m_Buffer.bindBufferBase(BindingPoint);
	}//bind

	uint32_t UBOMorphTargetData::size(void)const {
		uint32_t Rval = 0;
		Rval += 4 * sizeof(int32_t); // Data
		Rval += 3 * 4 * sizeof(int32_t); // activation IDs
		Rval += 3 * 4 * sizeof(float); // activation strengths
		return Rval;
	}//size

	void UBOMorphTargetData::setMorphTargetParam(uint32_t Index, uint32_t MorphTargetID, float Strength) {
		if (Index > 11) throw IndexOutOfBoundsExcept("Morph Target index out of bounds!");
		uint32_t Offset = m_ActivationIDsOffset + Index * sizeof(int32_t);
		m_Buffer.bufferSubData(Offset, sizeof(int32_t), (const void*)&MorphTargetID);
		Offset = m_ActivationStrengthsOffset + Index * sizeof(float);
		m_Buffer.bufferSubData(Offset, sizeof(float), (const void*)&Strength);

	}//setMorphTargetParam

	void UBOMorphTargetData::activeMorphTargets(int32_t Count) {
		m_Buffer.bufferSubData(m_DataOffset + sizeof(int32_t), sizeof(int32_t), (const void*)&Count);
	}//activeMorphTargets

	void UBOMorphTargetData::dataOffset(int32_t DataOffset) {
		m_Buffer.bufferSubData(m_DataOffset, sizeof(int32_t), (const void*)&DataOffset);
	}//dataOffset

}//name space