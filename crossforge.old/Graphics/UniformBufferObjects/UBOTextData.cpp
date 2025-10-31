#include "UBOTextData.h"
using namespace Eigen;

namespace CForge {

	UBOTextData::UBOTextData(void) : CForgeObject("UBOTextData") {
		m_TextPositionOffset = 0;
		m_CanvasSizeOffset = 0;
		m_ColorOffset = 0;
	}//Constructor

	UBOTextData::~UBOTextData(void) {
		clear();
	}//Destructor

	void UBOTextData::init(void) {
		clear();

		m_Buffer.init(GLBuffer::BTYPE_UNIFORM, GLBuffer::BUSAGE_STATIC_DRAW, nullptr, size());

		m_ColorOffset = 0;	
		m_CanvasSizeOffset = sizeof(float) * 4;
		m_TextPositionOffset = m_CanvasSizeOffset + 2 * sizeof(float);
	}//initialize

	void UBOTextData::clear(void) {
		m_TextPositionOffset = 0;
		m_CanvasSizeOffset = 0;
		m_ColorOffset = 0;
		m_Buffer.clear();
	}//clear

	void UBOTextData::bind(uint32_t BindingPoint) {
		m_Buffer.bindBufferBase(BindingPoint);
	}//bind


	void UBOTextData::color(const Eigen::Vector4f Color) {
		m_Buffer.bufferSubData(m_ColorOffset, 4 * sizeof(float), Color.data());
	}//position

	void UBOTextData::canvasSize(Eigen::Vector2f CanvasSize) {
		m_Buffer.bufferSubData(m_CanvasSizeOffset, 2 * sizeof(float), CanvasSize.data());
	}//canvasSize

	void UBOTextData::textPosition(Eigen::Vector2f TextPosition) {
		m_Buffer.bufferSubData(m_TextPositionOffset, 2 * sizeof(float), TextPosition.data());
	}//textPosition

	uint32_t UBOTextData::size(void)const {
		uint32_t Rval = 0;
		Rval += 4 * sizeof(float); // Color data
		Rval += 4 * sizeof(float); // Canvas size and text position
		return Rval;
	}//size

}//name space