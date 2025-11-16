#include <glad/glad.h>
#include "GraphicsUtility.h"
#include "../Graphics/controllers/Image2DController.h"

using namespace Eigen;
namespace crossforge {

	uint32_t GraphicsUtility::checkGLError(std::string* pVerbose) {
		std::string ErrorMsg;

		uint32_t Rval = glGetError();
		if (GL_NO_ERROR != Rval) {
			switch (Rval) {
			case GL_INVALID_ENUM: {
				ErrorMsg = "Invalid Enum";
			}break;
			case GL_INVALID_VALUE: {
				ErrorMsg = "Invalid value";
			}break;
			case GL_INVALID_OPERATION: {
				ErrorMsg = "Invalid operation";
			}break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: {
				ErrorMsg = "Invalid Framebuffer operation";
			}break;
			case GL_OUT_OF_MEMORY: {
				ErrorMsg = "Out of Memory";
			}break;
			case GL_STACK_UNDERFLOW: {
				ErrorMsg = "Stack Underflow";
			}break;
			case GL_STACK_OVERFLOW: {
				ErrorMsg = "Stack Overflow";
			}break;
			default: {
				ErrorMsg = "Unknown";
			}break;
			}
		}//if[gl error occurred]

		if (nullptr != pVerbose) (*pVerbose) = ErrorMsg;

		return Rval;
	}//checkGLError

	bool GraphicsUtility::retrieveColorTexture(uint32_t texObj, Image2DEntityPtr pImage, uint32_t level) {
#ifndef __EMSCRIPTEN__
		if (nullptr == pImage) throw NullpointerExcept("pImage");
		if (!glIsTexture(texObj)) throw CrossForgeExcept("Specified texture object with id " + std::to_string(texObj) + " is not a valid OpenGL texture.");
		bool result = false;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texObj);

		int32_t texWidth = 0;
		int32_t texHeight = 0;

		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &texWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &texHeight);

		if (texWidth == 0 || texHeight == 0) {
			LogError("Retrieved image dimensions of " + std::to_string(texWidth) + " x " + std::to_string(texHeight) + " are not valid!");
		}
		else {
			try {
				std::vector<uint8_t> buffer;
				buffer.resize(texWidth * texHeight * 3);
				glGetTexImage(GL_TEXTURE_2D, level, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
				auto pImgDataComp = pImage->getImage2DComponent(true);
				pImgDataComp->clear();
				pImgDataComp->colorSpace() = Image2DComponent::COLORSPACE_RGB;
				pImgDataComp->width() = texWidth;
				pImgDataComp->height() = texHeight;
				pImgDataComp->pixelData() = buffer;

				Image2DController::flipRows(pImage);
				result = true;
			}
			catch (CrossForgeException& e) {
				Logger::logException(e);
			}
		}
		return result;
#endif
	}//retrieveDeptBuffer

	bool GraphicsUtility::retrieveDepthTexture(uint32_t texObj, Image2DEntityPtr pImage, uint32_t level, float near, float far) {
#ifndef __EMSCRIPTEN__
		if (nullptr == pImage) throw NullpointerExcept("pImage");
		if (!glIsTexture(texObj)) throw CrossForgeExcept("Specified object with id " + std::to_string(texObj) + " is not a valid OpenGL texture.");
		bool result = false;

		glBindTexture(GL_TEXTURE_2D, texObj);
		int32_t texWidth = 0;
		int32_t texHeight = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &texWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &texHeight);

		if (0 == texWidth || 0 == texHeight) {
			LogError("Retrieved image dimensions of " + std::to_string(texWidth) + " x " + std::to_string(texHeight) + " are not valid.");
		}
		else {
			std::vector<float> buffer;
			buffer.resize(texWidth * texHeight);

			glGetTexImage(GL_TEXTURE_2D, level, GL_DEPTH_COMPONENT, GL_FLOAT, buffer.data());

			// linearize depth?
			if (near > 0.0f && far > 0.0f) {
				for (uint32_t i = 0; i < texWidth * texHeight; ++i) {
					float z = buffer[i] * 2.0f - 1.0f; // back to NDC
					buffer[i] = (2.0f * near * far) / (far + near - z * (far - near));
					buffer[i] /= (far - near);
				}//For[all depth values]
			}//if[linearize depth values]

			std::vector<uint8_t> imgBuffer;
			imgBuffer.resize(texWidth * texHeight * 3);
			// convert to RGB
			for (uint32_t i = 0; i < texWidth * texHeight; ++i) {
				imgBuffer[i * 3 + 0] = buffer[i] * 255.0f;
				imgBuffer[i * 3 + 1] = buffer[i] * 255.0f;
				imgBuffer[i * 3 + 2] = buffer[i] * 255.0f;
			}

			auto pDataComp = pImage->getImage2DComponent(true);
			pDataComp->colorSpace() = Image2DComponent::COLORSPACE_RGB;
			pDataComp->width() = texWidth;
			pDataComp->height() = texHeight;
			pDataComp->pixelData() = imgBuffer;
			Image2DController::flipRows(pImage);
			result = true;
		}
		return result;
#endif
	}//retrieveDeptBuffer


	uint32_t GraphicsUtility::gpuMemoryAvailable(void) {
		const uint32_t GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX = 0x9048;
		const uint32_t TEXTURE_FREE_MEMORY_ATI = 0x87FC;

		int32_t totalMemory[4] = { 0,0,0,0 };
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, totalMemory);
		if (checkGLError(nullptr) != 0) glGetIntegerv(TEXTURE_FREE_MEMORY_ATI, totalMemory);
		checkGLError(nullptr);

		return totalMemory[0];
	}//gpuMemoryAvailable

	uint32_t GraphicsUtility::gpuFreeMemory(void) {
		const uint32_t GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX = 0x9049;
		int32_t availableMemory = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &availableMemory);
		return availableMemory;
	}//gpuMemoryUsage

	bool GraphicsUtility::retrieveFrameBuffer(Image2DEntityPtr pColorImage, Image2DEntityPtr pDepthImage, float near, float far) {

		// get framebuffer width and height
		int32_t Viewport[4];
		glGetIntegerv(GL_VIEWPORT, Viewport);

		int32_t x = Viewport[0];
		int32_t y = Viewport[1];
		int32_t width = Viewport[2];
		int32_t height = Viewport[3];


		if (nullptr != pColorImage) {
			std::vector<uint8_t> buffer;
			buffer.resize(width * height * 3);

			glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

			auto pImgComp = pColorImage->getImage2DComponent(true);
			pImgComp->clear();
			pImgComp->colorSpace() = Image2DComponent::COLORSPACE_RGB;
			pImgComp->width() = width;
			pImgComp->height() = height;
			pImgComp->pixelData() = buffer;
			Image2DController::flipRows(pColorImage);
		}

		if (nullptr != pDepthImage) {

			std::vector<uint8_t> buffer;
			std::vector<float> depthBuffer;
			buffer.resize(width * height);
			depthBuffer.resize(width * height);
			glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depthBuffer.data());

			// linearize depth?
			if (near > 0.0f && far > 0.0f) {
				for (uint32_t i = 0; i < width * height; ++i) {
					float z = depthBuffer[i] * 2.0f - 1.0f; // back to NDC
					depthBuffer[i] = (2.0f * near * far) / (far + near - z * (far - near));
					depthBuffer[i] /= (far - near);
				}//For[all depth values]
			}//if[linearize depth values]

			for (uint32_t i = 0; i < width * height; ++i) buffer[i] = (uint8_t)(depthBuffer[i] * 255.0f);


			auto pImgComp = pDepthImage->getImage2DComponent(true);
			pImgComp->clear();
			pImgComp->colorSpace() = Image2DComponent::COLORSPACE_GRAYSCALE;
			pImgComp->width() = width;
			pImgComp->height() = height;
			pImgComp->pixelData() = buffer;
			Image2DController::flipRows(pDepthImage);
		}

		return true;

	}//fetchFramebuffer

	GraphicsUtility::GPUTraits GraphicsUtility::retrieveGPUTraits(void) {
		GPUTraits result;

		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &result.maxTextureImageUnits);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &result.maxFragmentUniformComponents);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &result.maxFragmentUniformBlocks);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &result.maxUniformBlockSize);
		glGetIntegerv(GL_MAX_VARYING_VECTORS, &result.maxVaryingVectors);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &result.maxVertexAttribs);

#ifndef __EMSCRIPTEN__
		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &result.maxFramebufferWidth);
		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &result.maxFramebufferHeight);
#endif
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &result.maxColorAttachements);

		GLint minorVersion = 0;
		GLint majorVersion = 0;
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		result.glMinorVersion = minorVersion;
		result.glLMajorVersion = majorVersion;

		result.glVersion = (const char*)glGetString(GL_VERSION);

		return result;
	}//retrieveGPUTraits

	

	void GraphicsUtility::defaultMaterial(PbrMaterialPtr pMaterial, DefaultMaterial mat) {
		if (nullptr == pMaterial) throw NullpointerExcept("pMaterial");
		if (mat <= MATERIAL_UNKNOWN || mat >= DEFAULT_MATERIAL_COUNT) throw IndexOutOfBoundsExcept("mat");

		switch (mat) {
		case METAL_GOLD: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0xe4, 0x49, 0xff) / 255.0f;
			pMaterial->roughness() = 0.35f;
			pMaterial->metallic() = 1.0f;
		}break;
		case METAL_SILVER: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xb1, 0xba, 0xbd, 0xff) / 255.0f;
			pMaterial->roughness() = 0.3f;
			pMaterial->metallic() = 1.0f;
		}break;
		case METAL_COPPER: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xcd, 0x62, 0x33, 0xff) / 255.0f;
			pMaterial->roughness() = 0.4f;
			pMaterial->metallic() = 1.0f;
		}break;
		case METAL_IRON: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x63, 0x66, 0x61, 0xff) / 255.0f;
			pMaterial->roughness() = 0.3f;
			pMaterial->metallic() = 1.0f;
		}break;
		case METAL_STEEL: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xaf, 0xb8, 0xb5, 0xff) / 255.0f;
			pMaterial->roughness() = 0.3f;
			pMaterial->metallic() = 1.0f;
		}break;
		case METAL_STAINLESS_STEEL: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xca, 0xcc, 0xcf, 0xff) / 255.0f;
			pMaterial->roughness() = 0.3f;
			pMaterial->metallic() = 1.0f;
		}break;
		case METAL_WHITE: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0xff, 0xff, 0xff) / 255.0f;
			pMaterial->roughness() = 0.3f;
			pMaterial->metallic() = 1.0f;
		}break;
		case METAL_RED: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0x00, 0x00, 0xff) / 255.0f;
			pMaterial->roughness() = 0.3f;
			pMaterial->metallic() = 1.0f;
		}break;
		case METAL_BLUE: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x00, 0x00, 0xff, 0xff) / 255.0f;
			pMaterial->roughness() = 0.3f;
			pMaterial->metallic() = 1.0f;
		}break;
		case METAL_GREEN: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x00, 0xff, 0x00, 0xff) / 255.0f;
			pMaterial->roughness() = 0.3f;
			pMaterial->metallic() = 1.0f;
		}break;


		case PLASTIC_WHITE: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0xff, 0xff, 0xff) / 255.0f;
			pMaterial->roughness() = 0.15f;
			pMaterial->metallic() = 0.04f;
		}break;
		case PLASTIC_GRAY: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x90, 0x90, 0x90, 0xff) / 255.0f;
			pMaterial->roughness() = 0.15f;
			pMaterial->metallic() = 0.04f;
		}break;
		case PLASTIC_BLACK: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x00, 0x00, 0x00, 0xff) / 255.0f;
			pMaterial->roughness() = 0.15f;
			pMaterial->metallic() = 0.04f;
		}break;
		case PLASTIC_RED: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0x00, 0x00, 0xff) / 255.0f;
			pMaterial->roughness() = 0.15f;
			pMaterial->metallic() = 0.04f;
		}break;
		case PLASTIC_GREEN: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x00, 0xff, 0x00, 0xff) / 255.0f;
			pMaterial->roughness() = 0.15f;
			pMaterial->metallic() = 0.04f;
		}break;
		case PLASTIC_BLUE: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x00, 0x00, 0xff, 0xff) / 255.0f;
			pMaterial->roughness() = 0.15f;
			pMaterial->metallic() = 0.04f;
		}break;
		case PLASTIC_YELLOW: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0xe4, 0x49, 0xff) / 255.0f;
			pMaterial->roughness() = 0.15f;
			pMaterial->metallic() = 0.04f;
		}break;

		case STONE_WHITE: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0xff, 0xff, 0xff) / 255.0f;
			pMaterial->roughness() = 0.8f;
			pMaterial->metallic() = 0.04f;
		}break;
		case STONE_GRAY: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x90, 0x90, 0x90, 0xff) / 255.0f;
			pMaterial->roughness() = 0.8f;
			pMaterial->metallic() = 0.04f;
		}break;
		case STONE_BLACK: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x00, 0x00, 0x00, 0xff) / 255.0f;
			pMaterial->roughness() = 0.8f;
			pMaterial->metallic() = 0.04f;
		}break;
		case STONE_RED: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0x00, 0x00, 0xff) / 255.0f;
			pMaterial->roughness() = 0.8f;
			pMaterial->metallic() = 0.04f;
		}break;
		case STONE_GREEN: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x00, 0xff, 0x00, 0xff) / 255.0f;
			pMaterial->roughness() = 0.8f;
			pMaterial->metallic() = 0.04f;
		}break;
		case STONE_BLUE: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0x00, 0x00, 0xff, 0xff) / 255.0f;
			pMaterial->roughness() = 0.8f;
			pMaterial->metallic() = 0.04f;
		}break;
		case STONE_YELLOW: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0xe4, 0x49, 0xff) / 255.0f;
			pMaterial->roughness() = 0.8f;
			pMaterial->metallic() = 0.04f;
		}break;
		default: {
			pMaterial->color(PbrMaterial::COLOR_TYPE_ALBEDO) = Vector4f(0xff, 0xff, 0xff, 0xff) / 255.0f;
			pMaterial->roughness() = 0.8f;
			pMaterial->metallic() = 0.04f;
		};
		}//switch[Mat]


	}//retrieveMaterialDefinition


	GraphicsUtility::GraphicsUtility() {

	}
	GraphicsUtility::~GraphicsUtility() {

	}
}