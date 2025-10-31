#include "UBOColorAdjustment.h"

namespace CForge {

	UBOColorAdjustment::UBOColorAdjustment(void): CForgeObject("UBOColorAdjustment") {

	}//Constructor

	UBOColorAdjustment::~UBOColorAdjustment(void) {
		clear();
	}//Destructor

	void UBOColorAdjustment::init(void) {
		clear();

		uint32_t Size = sizeof(float) * 4;
		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_STATIC_DRAW, nullptr, Size);

		Eigen::Vector4f Vals = Eigen::Vector4f::Ones();
		m_Buffer.bufferSubData(0, sizeof(float) * 4, Vals.data());

	}//initialize

	void UBOColorAdjustment::clear(void) {
		m_Buffer.clear();
	}//clear

	void UBOColorAdjustment::bind(uint32_t BindingPoint) {
		m_Buffer.bindBufferBase(BindingPoint);
	}//bind


	void UBOColorAdjustment::contrast(float Contrast) {
		m_Buffer.bufferSubData(sizeof(float), sizeof(float), &Contrast);
	}//contrast

	void UBOColorAdjustment::saturation(float Saturation) {
		m_Buffer.bufferSubData(0, sizeof(float), &Saturation);
	}//saturation

	void UBOColorAdjustment::brigthness(float Brightness) {
		m_Buffer.bufferSubData(sizeof(float)*2, sizeof(float), &Brightness);
	}//brightness

}//name space