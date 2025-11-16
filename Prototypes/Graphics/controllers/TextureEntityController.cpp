#include <glad/glad.h>
#include "TextureEntityController.h"
#include "../../utility/GraphicsUtility.h"
#include "Image2DController.h"

namespace crossforge {


	TextureEntityController::TextureEntityController(const std::string childIdentification): ControllerBase(TextureEntityController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	TextureEntityController::~TextureEntityController() {

	}

	bool TextureEntityController::buildTexture2D(TextureEntityPtr pTexture, Image2DEntityPtr pImage, bool generateMitmaps) {
		if (nullptr == pTexture) throw NullpointerExcept("pTexture");
		if (nullptr == pImage) throw NullpointerExcept("pImage");

		auto pRawImage = pImage->getImage2DComponent();
		if (nullptr == pRawImage) throw MissingComponentException(Image2DComponent::identification);
		if (pRawImage->width() == 0 || pRawImage->height() == 0) {
			LogError("Image appears to have no valid data. Widtth and/or height are 0.");
			return false;
		}
		
		// get color space
		uint32_t glColorSpace = 0;
		switch (pRawImage->colorSpace()) {
		case Image2DComponent::COLORSPACE_GRAYSCALE: glColorSpace = GL_R; break;
		case Image2DComponent::COLORSPACE_RGB: glColorSpace = GL_RGB; break;
		case Image2DComponent::COLORSPACE_RGBA: glColorSpace = GL_RGBA; break;
		default: {
			LogError("Raw image has invalid color space " + std::to_string(pRawImage->colorSpace()));
			return false;
		}break;
		}

		uint32_t textureHandle = GL_INVALID_INDEX;
		// generate texture
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_2D, textureHandle);

		Image2DEntityPtr pFlippedImg = std::make_shared<Image2DEntity>();
		auto pRawImgData = pFlippedImg->getImage2DComponent(true);
		pRawImgData->colorSpace() = pRawImage->colorSpace();
		pRawImgData->width() = pRawImage->width();
		pRawImgData->height() = pRawImage->height();
		pRawImgData->pixelData() = pRawImage->pixelData();
		Image2DController::flipRows(pFlippedImg);

		glTexImage2D(GL_TEXTURE_2D, 0, glColorSpace,pRawImgData->width(), pRawImgData->height(), 0, glColorSpace, GL_UNSIGNED_BYTE, (const void*)pRawImgData->pixelData().data());

		if (generateMitmaps) {
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		// set texture parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		std::string glErrorText = "";
		uint32_t glError = GraphicsUtility::checkGLError(&glErrorText);
		if (GL_NO_ERROR != glError) LogError("OpenGL error occurred during texture generation: " + glErrorText);

		// store data to entity
		if (!pTexture->hasComponent(Texture2DComponent::identification)) pTexture->addComponent(std::make_shared<Texture2DComponent>());
		auto pTexture2DComp = pTexture->getTexture2DComponent();

		pTexture2DComp->glTextureHandle() = textureHandle;
		pTexture2DComp->width() = pRawImage->width();
		pTexture2DComp->height() = pRawImage->height();

		return (GL_NO_ERROR == glError);


	}
}