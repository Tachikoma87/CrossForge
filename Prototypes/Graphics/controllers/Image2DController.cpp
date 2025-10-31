#include "Image2DController.h"

namespace crossforge {

	Image2DController::Image2DController(const std::string childIdentification) : ControllerBase(Image2DController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	Image2DController::~Image2DController() {

	}

	bool Image2DController::fliprRows(Image2DEntityPtr pImage2D) {
		if (nullptr == pImage2D) throw NullpointerExcept("pImage2D");
		auto pRawData = pImage2D->getRawImage2DDataComponent();
		if (nullptr == pRawData) throw MissingComponentException(RawImage2DDataComponent::identification);

		std::vector<uint8_t> newPixelData;
		uint32_t rowSize = pRawData->width() * pRawData->getBytesPerPixel();
		newPixelData.resize(pRawData->width() * pRawData->height() * pRawData->getBytesPerPixel());

		for (uint32_t i = 0; i < pRawData->width(); ++i) {
			uint32_t indexOrig = i * rowSize;
			uint32_t indexNew = (pRawData->height() - i - 1) * rowSize;
			memcpy(&newPixelData.data()[indexNew], &pRawData->rawPixelData()[indexOrig], rowSize * sizeof(uint8_t));
		}
		pRawData->rawPixelData() = newPixelData;
		return true;
	}

	bool Image2DController::rotate90Degree(Image2DEntityPtr pImage2D) {
		if (nullptr == pImage2D) throw NullpointerExcept("pImage2D");
		auto pRawImgData = pImage2D->getRawImage2DDataComponent();
		if (nullptr == pRawImgData) throw MissingComponentException(RawImage2DDataComponent::identification);

		std::vector<uint8_t> newPixelData;
		newPixelData.resize( pRawImgData->getImageSize());

		const uint32_t width = pRawImgData->width();
		const uint32_t height = pRawImgData->height();
		const uint32_t bpp = pRawImgData->getBytesPerPixel();

		for (uint32_t r = 0; r < width; ++r) {
			for (uint32_t c = 0; c < height; ++c) {
				for (uint8_t p = 0; p < bpp; ++p) {
					newPixelData[((width - r - 1) * height + c) * bpp + p] = pRawImgData->rawPixelData()[(c * width + r) * bpp + p];
				}
			}
		}

		pRawImgData->width() = height;
		pRawImgData->height() = width;
		pRawImgData->rawPixelData() = newPixelData;

		return true;

	}
	bool Image2DController::rotate180Degree(Image2DEntityPtr pImage2D) {
		bool result = false;
		try {
			result = rotate90Degree(pImage2D);
			result &= rotate90Degree(pImage2D);
		}
		catch (CrossForgeException e) {
			Logger::logException(e);
		}
		return result;

	}
	bool Image2DController::rotate270Degree(Image2DEntityPtr pImage2D) {
		bool result = false;
		try {
			result = rotate90Degree(pImage2D);
			result &= rotate90Degree(pImage2D);
			result &= rotate90Degree(pImage2D);
		}
		catch (CrossForgeException e) {
			Logger::logException(e);
		}
		return result;

	}

}