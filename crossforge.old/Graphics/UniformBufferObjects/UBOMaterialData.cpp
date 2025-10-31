#include "UBOMaterialData.h"

namespace CForge {

	UBOMaterialData::UBOMaterialData(void): CForgeObject("UBOMaterialData") {
		m_ColorOffset = 0;
		m_MetallicOffset = 0;
		m_RoughnessOffset = 0;
		m_AmbientOcclusionOffset = 0;
	}//Constructor

	UBOMaterialData::~UBOMaterialData(void) {
		clear();
	}//Destructor

	void UBOMaterialData::init(void) {
		clear();
		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_STATIC_DRAW, nullptr, size());
		m_ColorOffset = 0;
		m_MetallicOffset = sizeof(float) * 4;
		m_RoughnessOffset = m_MetallicOffset + sizeof(float);
		m_AmbientOcclusionOffset = m_RoughnessOffset + sizeof(float);
	}//initialize

	void UBOMaterialData::clear(void) {
		m_Buffer.clear();
		m_ColorOffset = 0;
		m_MetallicOffset = 0;
		m_RoughnessOffset = 0;
		m_AmbientOcclusionOffset = 0;
	}//clear

	void UBOMaterialData::bind(uint32_t BindingPoint) {
		m_Buffer.bindBufferBase(BindingPoint);
	}//bind

	uint32_t UBOMaterialData::size(void)const {
		uint32_t Rval = 0;
		Rval += sizeof(float) * 4; // color components
		Rval += sizeof(float) * 16; // Attributes + padding
		return Rval;
	}//size

	void UBOMaterialData::color(Eigen::Vector4f Color) {
		m_Buffer.bufferSubData(m_ColorOffset, sizeof(float) * 4, Color.data());
	}//color

	void UBOMaterialData::metallic(float Metallic) {
		m_Buffer.bufferSubData(m_MetallicOffset, sizeof(float), &Metallic);
	}//metallic

	void UBOMaterialData::roughness(float Roughness) {
		m_Buffer.bufferSubData(m_RoughnessOffset, sizeof(float), &Roughness);
	}//roughness

	void UBOMaterialData::ambientOcclusion(float AO) {
		m_Buffer.bufferSubData(m_AmbientOcclusionOffset, sizeof(float), &AO);
	}//ambientOcclusion

}//name space