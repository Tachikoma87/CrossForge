#include "Image2DController.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

#include "../../utility/MiscUtility.hpp"

namespace crossforge {

	Image2DController::Image2DController(const std::string childIdentification) : ControllerBase(Image2DController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	Image2DController::~Image2DController() {
		
	}

	bool Image2DController::flipRows(Image2DEntityPtr pImage2D) {
		if (nullptr == pImage2D) throw NullpointerExcept("pImage2D");
		auto pRawData = pImage2D->getImage2DComponent();
		if (nullptr == pRawData) throw MissingComponentException(Image2DComponent::identification);

		std::vector<uint8_t> newPixelData;
		uint32_t rowSize = pRawData->width() * pRawData->getBytesPerPixel();
		newPixelData.resize(pRawData->width() * pRawData->height() * pRawData->getBytesPerPixel());

		for (uint32_t i = 0; i < pRawData->height(); ++i) {
			uint32_t indexOrig = i * rowSize;
			uint32_t indexNew = (pRawData->height() - i - 1) * rowSize;
			memcpy(&newPixelData.data()[indexNew], &pRawData->pixelData()[indexOrig], rowSize * sizeof(uint8_t));
		}
		pRawData->pixelData() = newPixelData;
		return true;
	}

	bool Image2DController::rotate90Degree(Image2DEntityPtr pImage2D) {
		if (nullptr == pImage2D) throw NullpointerExcept("pImage2D");
		auto pRawImgData = pImage2D->getImage2DComponent();
		if (nullptr == pRawImgData) throw MissingComponentException(Image2DComponent::identification);

		std::vector<uint8_t> newPixelData;
		newPixelData.resize( pRawImgData->getImageSize());

		const uint32_t width = pRawImgData->width();
		const uint32_t height = pRawImgData->height();
		const uint32_t bpp = pRawImgData->getBytesPerPixel();

		for (uint32_t r = 0; r < width; ++r) {
			for (uint32_t c = 0; c < height; ++c) {
				for (uint8_t p = 0; p < bpp; ++p) {
					newPixelData[((width - r - 1) * height + c) * bpp + p] = pRawImgData->pixelData()[(c * width + r) * bpp + p];
				}
			}
		}

		pRawImgData->width() = height;
		pRawImgData->height() = width;
		pRawImgData->pixelData() = newPixelData;

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

	bool Image2DController::generateImage(Image2DEntityPtr pImage2D, uint32_t width, uint32_t height, Eigen::Vector3f color) {
		if (nullptr == pImage2D) throw NullpointerExcept("pImage2D");
		bool result = false;
		if (0 == width || 0 == height) {
			LogError("Width and/or height is 0. Not a valid image");
		}
		else {

			auto pRawImageData = pImage2D->getImage2DComponent(true);
			pRawImageData->clear();
			pRawImageData->colorSpace() = Image2DComponent::COLORSPACE_RGB;
			pRawImageData->width() = width;
			pRawImageData->height() = height;
			auto &buffer = pRawImageData->pixelData();
			buffer.resize(width * height * 3);
			for (uint32_t i = 0; i < width * height; ++i) {
				buffer[i * 3 + 0] = (uint8_t)(color.x()*255.0f);
				buffer[i * 3 + 1] = (uint8_t)(color.y()*255.0f);
				buffer[i * 3 + 2] = (uint8_t)(color.z()*255.0f);
			}
			result = true;
		}
		return result;
	}

	bool Image2DController::generateBasicImage(Image2DEntityPtr pImage2D, BasicImage basicImage) {
		if (nullptr == pImage2D) throw NullpointerExcept("pImage");
		if (basicImage <= BASIC_IMAGE_UNKNOWN || basicImage >= BASIC_IMAGE_COUNT) throw IndexOutOfBoundsExcept("basicImage");

		switch (basicImage) {
		case BASIC_IMAGE_8X8_RED:	generateImage(pImage2D, 8, 8, Eigen::Vector3f(1.0f, 0.0f, 0.0f)); break;
		case BASIC_IMAGE_8X8_GREEN: generateImage(pImage2D, 8, 8, Eigen::Vector3f(0.0f, 1.0f, 0.0f)); break;
		case BASIC_IMAGE_8X8_BLUE:	generateImage(pImage2D, 8, 8, Eigen::Vector3f(0.0f, 0.0f, 1.0f)); break;
		case BASIC_IMAGE_8X8_WHITE:	generateImage(pImage2D, 8, 8, Eigen::Vector3f(1.0f, 1.0f, 1.0f)); break;
		case BASIC_IMAGE_8X8_BLACK: generateImage(pImage2D, 8, 8, Eigen::Vector3f(0.0f, 0.0f, 0.0f)); break;
		default: {
			LogError("Not handled basic image enumerate encountered.");
			generateImage(pImage2D, 8, 8, Eigen::Vector3f::Ones());
		}break;
		}
		return true;
	}

	Image2DEntityPtr Image2DController::generateBasicImage(BasicImage basicImage) {
		Image2DEntityPtr pResult = std::make_shared<Image2DEntity>();
		return (generateBasicImage(pResult, basicImage)) ? pResult : nullptr;
	}

	bool Image2DController::resize(Image2DEntityPtr pImage2D, uint32_t width, uint32_t height) {
		if (nullptr == pImage2D) throw NullpointerExcept("pImage2D");
		auto pRawImageComp = pImage2D->getImage2DComponent();
		if (nullptr == pRawImageComp) throw MissingComponentException(Image2DComponent::identification);
		bool result = false;

		if (0 == pRawImageComp->width() || 0 == pRawImageComp->height()) LogError("Width of height of image is 0. Can not resize it.");
		else if (0 == pRawImageComp->pixelData().size()) LogError("Image does not cotain any data!");
		else {
			stbir_pixel_layout pixelLayout;
			switch (pRawImageComp->colorSpace()) {
			case Image2DComponent::COLORSPACE_GRAYSCALE: pixelLayout = stbir_pixel_layout::STBIR_1CHANNEL; break;
			case Image2DComponent::COLORSPACE_RGB: pixelLayout = stbir_pixel_layout::STBIR_RGB; break;
			case Image2DComponent::COLORSPACE_RGBA: pixelLayout = stbir_pixel_layout::STBIR_RGBA; break;
			default: {
				LogError("Not handled color space of " + std::to_string(pRawImageComp->colorSpace()) + " encountered.");
				return result;
			}break;
			}

			std::vector<uint8_t> buffer;
			buffer.resize(width * height * pRawImageComp->getBytesPerPixel());
			stbir_resize_uint8_linear(pRawImageComp->pixelData().data(), pRawImageComp->width(), pRawImageComp->height(), 0,
				buffer.data(), width, height, 0, pixelLayout);

			pRawImageComp->width() = width;
			pRawImageComp->height() = height;
			pRawImageComp->pixelData() = buffer;
			result = true;
		}

		return result;
	}

	bool Image2DController::changeColorSpace(Image2DEntityPtr pImage2D, Image2DComponent::ColorSpace colorSpace) {
		if (nullptr == pImage2D) throw NullpointerExcept("pImage2D");
		if (Image2DComponent::COLORSPACE_UNKNOWN >= colorSpace || colorSpace >= Image2DComponent::COLORSPACE_COUNT) throw IndexOutOfBoundsExcept("colorSpace");
		auto pRawImageComp = pImage2D->getImage2DComponent();
		if (nullptr == pRawImageComp) throw MissingComponentException(Image2DComponent::identification);

		bool result = false;
		std::vector<uint8_t> buffer;
		std::vector<uint8_t>& origBuffer = pRawImageComp->pixelData();
		uint32_t width = pRawImageComp->width();
		uint32_t height = pRawImageComp->height();
		if (0 == pRawImageComp->width() || 0 == pRawImageComp->height()) LogError("Image width or height is 0.");
		else if (0 == pRawImageComp->pixelData().size()) LogError("Image contains no data.");
		else if (Image2DComponent::COLORSPACE_UNKNOWN == pRawImageComp->colorSpace()) LogError("Image has invalid colors space specified.");
		else if (pRawImageComp->colorSpace() == Image2DComponent::COLORSPACE_RGBA && colorSpace == Image2DComponent::COLORSPACE_RGB) {
			buffer.resize(width * height * 3);
			for (uint32_t i = 0; i < width * height; ++i) {
				buffer[i * 3 + 0] = origBuffer[i * 4 + 0];
				buffer[i * 3 + 1] = origBuffer[i * 4 + 1];
				buffer[i * 3 + 2] = origBuffer[i * 4 + 2];
			}
		}
		else if (pRawImageComp->colorSpace() == Image2DComponent::COLORSPACE_RGBA && colorSpace == Image2DComponent::COLORSPACE_GRAYSCALE) {
			buffer.resize(width * height);
			for (uint32_t i = 0; i < width * height; ++i) {
				Eigen::Vector3f color(origBuffer[i * 4 + 0] / 255.0f, origBuffer[i * 4 + 1] / 255.0f, origBuffer[i * 4 + 2] / 255.0f);
				buffer[i] = MiscUtility::rgbToGrayscale(color)*255.0f;
			}
		}
		else if (pRawImageComp->colorSpace() == Image2DComponent::COLORSPACE_RGB && colorSpace == Image2DComponent::COLORSPACE_RGBA) {
			buffer.resize(width * height * 4);
			for (uint32_t i = 0; i < width * height; ++i) {
				buffer[i * 4 + 0] = origBuffer[i * 3 + 0];
				buffer[i * 4 + 1] = origBuffer[i * 3 + 1];
				buffer[i * 4 + 2] = origBuffer[i * 3 + 2];
				buffer[i * 4 + 3] = 255;
			}
		}
		else if (pRawImageComp->colorSpace() == Image2DComponent::COLORSPACE_RGB && colorSpace == Image2DComponent::COLORSPACE_GRAYSCALE) {
			buffer.resize(width * height);
			for (uint32_t i = 0; i < width * height; ++i) {
				Eigen::Vector3f color(origBuffer[i * 3 + 0] / 255.0f, origBuffer[i * 3 + 1] / 255.0f, origBuffer[i * 3 + 2] / 255.0f);
				buffer[i] = MiscUtility::rgbToGrayscale(color) * 255.0f;
			}
		}
		else if (pRawImageComp->colorSpace() == Image2DComponent::COLORSPACE_GRAYSCALE && colorSpace == Image2DComponent::COLORSPACE_RGBA) {
			buffer.resize(width * height * 4);
			for (uint32_t i = 0; i < width * height; ++i) {
				buffer[i * 4 + 0] = origBuffer[i];
				buffer[i * 4 + 1] = origBuffer[i];
				buffer[i * 4 + 2] = origBuffer[i];
				buffer[i * 4 + 3] = 255;
			}
		}
		else if (pRawImageComp->colorSpace() == Image2DComponent::COLORSPACE_GRAYSCALE && colorSpace == Image2DComponent::COLORSPACE_RGB) {
			buffer.resize(width * height * 3);
			for (uint32_t i = 0; i < width * height; ++i) {
				buffer[i * 3 + 0] = origBuffer[i];
				buffer[i * 3 + 1] = origBuffer[i];
				buffer[i * 3 + 2] = origBuffer[i];
			}
		}
		else {
			LogError("Not handled constellation encountered. Unable to determine how to convert image.");
		}

		if (buffer.size() > 0) {
			pRawImageComp->pixelData() = buffer;
			pRawImageComp->colorSpace() = colorSpace;
			result = true;
		}
		return result;
	}

}