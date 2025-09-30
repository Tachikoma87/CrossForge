#include "RawImage2DDataComponent.h"

namespace crossforge {
	RawImage2DDataComponent::RawImage2DDataComponent(): ComponentBase(RawImage2DDataComponent::identification) {
		m_inheritance.push_back(RawImage2DDataComponent::identification);
		initialize();
	}
	RawImage2DDataComponent::~RawImage2DDataComponent() {
		clear();
	}

	void RawImage2DDataComponent::initialize() {
		clear();
	}
	void RawImage2DDataComponent::clear() {
		m_width = -1;
		m_height = -1;
		m_colorSpace = COLORSPACE_UNKNOWN;
		m_rawPixelData.clear();
	}

	int32_t& RawImage2DDataComponent::width() {
		return m_width;
	}
	int32_t& RawImage2DDataComponent::height() {
		return m_height;
	}
	int8_t RawImage2DDataComponent::getBitsPerPixel()const {
		int8_t result = 0;
		switch (m_colorSpace) {
		case COLORSPACE_GRAYSCALE: result = 8; break;
		case COLORSPACE_RGB: result = 24; break;
		case COLORSPACE_RGBA: result = 32; break;
		default: break;
		}
		return result;
	}
	int8_t RawImage2DDataComponent::getBytesPerPixel()const {
		return getBitsPerPixel() / 8;
	}
	RawImage2DDataComponent::ColorSpace& RawImage2DDataComponent::colorSpace() {
		return m_colorSpace;
	}
	std::vector<uint8_t> &RawImage2DDataComponent::rawPixelData() {
		return m_rawPixelData;
	}

	uint64_t RawImage2DDataComponent::getImageSize()const {
		return m_width * m_height * getBytesPerPixel();
	}
}