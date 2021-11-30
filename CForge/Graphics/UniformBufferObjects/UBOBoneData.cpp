#include "UBOBoneData.h"

namespace CForge {

	UBOBoneData::UBOBoneData(void): CForgeObject("UBOBoneData") {

	}//Constructor

	UBOBoneData::~UBOBoneData(void) {

	}//Destructor

	void UBOBoneData::init(uint32_t BoneCount) {
		clear();
		m_BoneCount = BoneCount;
		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_DYNAMIC_DRAW, nullptr, size());
	}//initialize

	void UBOBoneData::clear(void) {
		m_Buffer.clear();
		m_BoneCount = 0;
	}//clear

	void UBOBoneData::bind(uint32_t BindingPoint) {
		m_Buffer.bindBufferBase(BindingPoint);
	}//bind

	uint32_t UBOBoneData::size(void)const {
		return m_BoneCount * 16 * sizeof(float);
	}//size

	void UBOBoneData::skinningMatrix(uint32_t Index, Eigen::Matrix4f SkinningMat) {
		if (Index >= m_BoneCount) throw IndexOutOfBoundsExcept("Index");
		uint32_t Offset = Index * 16 * sizeof(float);
		m_Buffer.bufferSubData(Offset, sizeof(float) * 16, SkinningMat.data());
	}//skinningMatrix

}//name-space