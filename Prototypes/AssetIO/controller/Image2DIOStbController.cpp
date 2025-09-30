#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include "../../utility/FileUtility.h"
#include "Image2DIOStbController.h"

namespace crossforge {
	Image2DIOStbController::Image2DIOStbController(): Image2DIOControllerBase(Image2DIOStbController::identification) {

	}
	Image2DIOStbController::~Image2DIOStbController() {

	}
	Image2DIOStbController::Image2DIOStbController(const std::string identification): Image2DIOControllerBase(Image2DIOStbController::identification) {
		m_inheritance.push_back(identification);
	}

	bool Image2DIOStbController::load(Image2DEntityPtr pImageEntity, const std::string filepath) {
		if (nullptr == pImageEntity) throw NullpointerExcept("pImageEntity");
		if (!FileUtility::exists(filepath)) {
			LogError("Image file " + filepath + " could not be found. Can not load image.");
			return false;
		}

		int32_t width = 0;
		int32_t height = 0;
		int32_t components = 0;
		uint8_t* pData = stbi_load(filepath.c_str(), &width, &height, &components, 0);

		if (nullptr == pData) {
			LogError("Trying to load image from file " + filepath + " returned no data. Imae loading failed!");
			return false;
		}

		RawImage2DDataComponent::ColorSpace cs = RawImage2DDataComponent::COLORSPACE_UNKNOWN;
		switch (components) {
		case 1: cs = RawImage2DDataComponent::COLORSPACE_GRAYSCALE; break;
		case 3: cs = RawImage2DDataComponent::COLORSPACE_RGB; break;
		case 4: cs = RawImage2DDataComponent::COLORSPACE_RGBA; break;
		default: {
			LogError("Image " + filepath + " reports " + std::to_string(components) + " components. This is unexpected and can not be handled.");
			stbi_image_free(pData);
			return false;
			break;
		}
		}

		if (!pImageEntity->hasComponent(RawImage2DDataComponent::identification)) pImageEntity->addComponent(std::make_shared<RawImage2DDataComponent>());
		auto pRawImageComp = pImageEntity->getRawImage2DDataComponent();
		pRawImageComp->colorSpace() = cs;
		pRawImageComp->width() = width;
		pRawImageComp->height() = height;
		int64_t imageSize = width * height * components;
		pRawImageComp->rawPixelData().resize(imageSize);
		memcpy(pRawImageComp->rawPixelData().data(), pData, imageSize);

		stbi_image_free(pData);
		pData = nullptr;
		return true;
	}

	bool Image2DIOStbController::store(Image2DEntityPtr pImageEntity, const std::string filepath) {

		if (nullptr == pImageEntity) throw NullpointerExcept("pImageEntity");
		if (!pImageEntity->hasComponent(RawImage2DDataComponent::identification)) throw MissingComponentException(RawImage2DDataComponent::identification);

		auto pImgDataComp = pImageEntity->getRawImage2DDataComponent();

		std::string str = GeneralUtility::toLowerCase(filepath);
		int32_t Rval = 0;

		if (str.find(".png") != std::string::npos) {
			Rval = stbi_write_png(filepath.c_str(), pImgDataComp->width(), pImgDataComp->height(), pImgDataComp->getBytesPerPixel(), pImgDataComp->rawPixelData().data(), 0);
		}
		else if (str.find(".bmp") != std::string::npos) {
			Rval = stbi_write_bmp(filepath.c_str(), pImgDataComp->width(), pImgDataComp->height(), pImgDataComp->getBytesPerPixel(), pImgDataComp->rawPixelData().data());
		}
		else if (str.find(".tga") != std::string::npos) {
			Rval = stbi_write_tga(filepath.c_str(), pImgDataComp->width(), pImgDataComp->height(), pImgDataComp->getBytesPerPixel(), pImgDataComp->rawPixelData().data());
		}
		else if (str.find(".jpg") != std::string::npos) {
			Rval = stbi_write_jpg(filepath.c_str(), pImgDataComp->width(), pImgDataComp->height(), pImgDataComp->getBytesPerPixel(), pImgDataComp->rawPixelData().data(), 90);
		}
		else if (str.find(".jpeg") != std::string::npos) {
			Rval = stbi_write_jpg(filepath.c_str(), pImgDataComp->width(), pImgDataComp->height(), pImgDataComp->getBytesPerPixel(), pImgDataComp->rawPixelData().data(), 80);
		}

		if (0 == Rval) {
			LogError("Something went wrong writing image " + filepath);
			return false;
		}
		return true;
	}
	bool Image2DIOStbController::canAcceptFile(std::string filePath, const Operation operation)const {
		bool Rval = false;
		std::string s = GeneralUtility::toLowerCase(filePath);

		if (operation == OP_LOAD) {
			if (s.find(".jpeg") != std::string::npos) Rval = true;
			if (s.find(".jpg") != std::string::npos) Rval = true;
			if (s.find(".png") != std::string::npos) Rval = true;
			if (s.find(".tga") != std::string::npos) Rval = true;
			if (s.find(".bmp") != std::string::npos) Rval = true;
			if (s.find(".psd") != std::string::npos) Rval = true;
			if (s.find(".pic") != std::string::npos) Rval = true;
		}
		else {
			if (s.find(".png") != std::string::npos) Rval = true;
			if (s.find(".tga") != std::string::npos) Rval = true;
			if (s.find(".bmp") != std::string::npos) Rval = true;
			if (s.find(".jpg") != std::string::npos) Rval = true;
			if (s.find(".jpeg") != std::string::npos) Rval = true;
		}

		return Rval;
	}
}