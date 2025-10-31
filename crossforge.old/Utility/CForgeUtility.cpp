#include <cuchar>

#include "../Graphics/OpenGLHeader.h"

#include "CForgeUtility.h"
#include "../Math/CForgeMath.h"
#include "../AssetIO/T3DMesh.hpp"
#include "../Graphics/RenderMaterial.h"
#include "../Graphics/Font/SFontManager.h"

using namespace Eigen;

namespace CForge {

	CForgeUtility::CForgeUtility(void): CForgeObject("CForgeUtility") {

	}//Constructor

	CForgeUtility::~CForgeUtility(void) {

	}//Destructor

	void CForgeUtility::retrieveColorTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg) {
#ifndef __EMSCRIPTEN__
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		if (!glIsTexture(TexObj)) throw CForgeExcept("Specified object is not a valid OpenGL texture.");
		glBindTexture(GL_TEXTURE_2D, TexObj);

		int32_t TexWidth = 0;
		int32_t TexHeight = 0;

		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &TexWidth);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_HEIGHT, &TexHeight);

		uint8_t* pBuffer = new uint8_t[TexWidth * TexHeight * 3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);

		pImg->init(TexWidth, TexHeight, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
		// imagme is flipped
		pImg->flipRows();

		delete[] pBuffer;
#endif
	}//retrieveDeptBuffer

	void CForgeUtility::retrieveDepthTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg, float Near, float Far) {
#ifndef __EMSCRIPTEN__
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		if (!glIsTexture(TexObj)) throw CForgeExcept("Specified object is not a valid OpenGL texture.");
		glBindTexture(GL_TEXTURE_2D, TexObj);
		int32_t TexWidth = 0;
		int32_t TexHeight = 0;
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &TexWidth);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_HEIGHT, &TexHeight);

		float* pDepthBuffer = new float[TexWidth * TexHeight];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pDepthBuffer);

		// linearize depth?
		if (Near > 0.0f && Far > 0.0f) {
			for (uint32_t i = 0; i < TexWidth * TexHeight; ++i) {
				float z = pDepthBuffer[i] * 2.0f - 1.0f; // back to NDC
				pDepthBuffer[i] = (2.0f * Near * Far) / (Far + Near - z * (Far - Near));
				pDepthBuffer[i] /= (Far - Near);
			}//For[all depth values]
		}//if[linearize depth values]

		uint8_t* pBuffer = new uint8_t[TexWidth * TexHeight * 3];
		// convert to RGB
		for (uint32_t i = 0; i < TexWidth * TexHeight; ++i) {
			pBuffer[i * 3 + 0] = pDepthBuffer[i] * 255.0f;
			pBuffer[i * 3 + 1] = pDepthBuffer[i] * 255.0f;
			pBuffer[i * 3 + 2] = pDepthBuffer[i] * 255.0f;
		}
		pImg->init(TexWidth, TexHeight, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
		pImg->flipRows();

		delete[] pBuffer;
		delete[] pDepthBuffer;
#endif
	}//retrieveDeptBuffer


	uint32_t CForgeUtility::checkGLError(std::string* pVerbose) {
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

	uint32_t CForgeUtility::gpuMemoryAvailable(void) {
		const uint32_t GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX = 0x9048;
		const uint32_t TEXTURE_FREE_MEMORY_ATI = 0x87FC;

		int32_t TotalMemory[4] = { 0,0,0,0 };
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, TotalMemory);
		if (checkGLError(nullptr) != 0) glGetIntegerv(TEXTURE_FREE_MEMORY_ATI, TotalMemory);
		checkGLError(nullptr);

		return TotalMemory[0];
	}//gpuMemoryAvailable

	uint32_t CForgeUtility::gpuFreeMemory(void) {
		const uint32_t GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX = 0x9049;
		int32_t AvailableMemory = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &AvailableMemory);
		return AvailableMemory;
	}//gpuMemoryUsage

	void CForgeUtility::retrieveFrameBuffer(T2DImage<uint8_t>* pColor, T2DImage<uint8_t>* pDepth, float Near, float Far) {

		// get framebuffer width and height
		int32_t Viewport[4];
		glGetIntegerv(GL_VIEWPORT, Viewport);

		int32_t x = Viewport[0];
		int32_t y = Viewport[1];
		int32_t Width = Viewport[2];
		int32_t Height = Viewport[3];

		//Width -= Width % 2;
		//Height -= Height % 2;
		//x = y = 0;

		if (nullptr != pColor) {
			uint8_t* pBuffer = new uint8_t[Width * Height * 4];
			glReadPixels(x, y, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);
			pColor->init(Width, Height, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
			pColor->flipRows();
			delete[] pBuffer;
		}

		if (nullptr != pDepth) {
			float* pBuffer = new float[Width * Height];
			uint8_t* pDepthBuffer = new uint8_t[Width * Height];
			glReadPixels(x, y, Width, Height, GL_DEPTH_COMPONENT, GL_FLOAT, pBuffer);

			// linearize depth?
			if (Near > 0.0f && Far > 0.0f) {
				for (uint32_t i = 0; i < Width * Height; ++i) {
					float z = pBuffer[i] * 2.0f - 1.0f; // back to NDC
					pBuffer[i] = (2.0f * Near * Far) / (Far + Near - z * (Far - Near));
					pBuffer[i] /= (Far - Near);
				}//For[all depth values]
			}//if[linearize depth values]

			for (uint32_t i = 0; i < Width * Height; ++i) pDepthBuffer[i] = (uint8_t)(pBuffer[i] * 255.0f);
			pDepth->init(Width, Height, T2DImage<uint8_t>::COLORSPACE_GRAYSCALE, pDepthBuffer);
			pDepth->flipRows();

			delete[] pBuffer;
			delete[] pDepthBuffer;
		}

	}//fetchFramebuffer

	CForgeUtility::GPUTraits CForgeUtility::retrieveGPUTraits(void) {
		GPUTraits Rval;

		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Rval.MaxTextureImageUnits);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &Rval.MaxFragmentUniformComponents);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &Rval.MaxFragmentUniformBlocks);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &Rval.MaxUniformBlockSize);
		glGetIntegerv(GL_MAX_VARYING_VECTORS, &Rval.MaxVaryingVectors);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &Rval.MaxVertexAttribs);

#ifndef __EMSCRIPTEN__
		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &Rval.MaxFramebufferWidth);
		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &Rval.MaxFramebufferHeight);
#endif
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &Rval.MaxColorAttachements);

		GLint MinorVersion = 0;
		GLint MajorVersion = 0;
		glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);
		glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
		Rval.GLMinorVersion = MinorVersion;
		Rval.GLMajorVersion = MajorVersion;

		Rval.GLVersion = (const char*)glGetString(GL_VERSION);

		return Rval;
	}//retrieveGPUTraits

	void CForgeUtility::defaultMaterial(T3DMesh<float>::Material* pMat, DefaultMaterial M) {
		if (nullptr == pMat) throw NullpointerExcept("pMat");
		MaterialDefinition Mat = retrieveMaterailDefinition(M);
		pMat->Color = Mat.Color;
		pMat->Roughness = Mat.Roughness;
		pMat->Metallic = Mat.Metallic;
	}//defaultMaterial

	void CForgeUtility::defaultMaterial(RenderMaterial* pMat, DefaultMaterial M) {
		if (nullptr == pMat) throw NullpointerExcept("pMat");
		MaterialDefinition Mat = retrieveMaterailDefinition(M);
		pMat->color(Mat.Color);
		pMat->roughness(Mat.Roughness);
		pMat->metallic(Mat.Metallic);
	}//defaultMaterial


	CForgeUtility::MaterialDefinition CForgeUtility::retrieveMaterailDefinition(DefaultMaterial Mat) {
		MaterialDefinition Rval;
		switch (Mat) {
		case METAL_GOLD: {
			Rval.Color = Vector4f(0xff, 0xe4, 0x49, 0xff) / 255.0f;
			Rval.Roughness = 0.35f;
			Rval.Metallic = 1.0f;
		}break;
		case METAL_SILVER: {
			Rval.Color = Vector4f(0xb1, 0xba, 0xbd, 0xff) / 255.0f;
			Rval.Roughness = 0.3f;
			Rval.Metallic = 1.0f;
		}break;
		case METAL_COPPER: {
			Rval.Color = Vector4f(0xcd, 0x62, 0x33, 0xff) / 255.0f;
			Rval.Roughness = 0.4f;
			Rval.Metallic = 1.0f;
		}break;
		case METAL_IRON: {
			Rval.Color = Vector4f(0x63, 0x66, 0x61, 0xff) / 255.0f;
			Rval.Roughness = 0.3f;
			Rval.Metallic = 1.0f;
		}break;
		case METAL_STEEL: {
			Rval.Color = Vector4f(0xaf, 0xb8, 0xb5, 0xff) / 255.0f;
			Rval.Roughness = 0.3f;
			Rval.Metallic = 1.0f;
		}break;
		case METAL_STAINLESS_STEEL: {
			Rval.Color = Vector4f(0xca, 0xcc, 0xcf, 0xff) / 255.0f;
			Rval.Roughness = 0.3f;
			Rval.Metallic = 1.0f;
		}break;
		case METAL_WHITE: {
			Rval.Color = Vector4f(0xff, 0xff, 0xff, 0xff) / 255.0f;
			Rval.Roughness = 0.3f;
			Rval.Metallic = 1.0f;
		}break;
		case METAL_RED: {
			Rval.Color = Vector4f(0xff, 0x00, 0x00, 0xff) / 255.0f;
			Rval.Roughness = 0.3f;
			Rval.Metallic = 1.0f;
		}break;
		case METAL_BLUE: {
			Rval.Color = Vector4f(0x00, 0x00, 0xff, 0xff) / 255.0f;
			Rval.Roughness = 0.3f;
			Rval.Metallic = 1.0f;
		}break;
		case METAL_GREEN: {
			Rval.Color = Vector4f(0x00, 0xff, 0x00, 0xff) / 255.0f;
			Rval.Roughness = 0.3f;
			Rval.Metallic = 1.0f;
		}break;

		
		case PLASTIC_WHITE: {
			Rval.Color = Vector4f(0xff, 0xff, 0xff, 0xff) / 255.0f;
			Rval.Roughness = 0.15f;
			Rval.Metallic = 0.04f;
		}break;
		case PLASTIC_GRAY: {
			Rval.Color = Vector4f(0x90,0x90,0x90, 0xff) / 255.0f;
			Rval.Roughness = 0.15f;
			Rval.Metallic = 0.04f;
		}break;
		case PLASTIC_BLACK: {
			Rval.Color = Vector4f(0x00, 0x00, 0x00, 0xff) / 255.0f;
			Rval.Roughness = 0.15f;
			Rval.Metallic = 0.04f;
		}break;
		case PLASTIC_RED: {
			Rval.Color = Vector4f(0xff, 0x00, 0x00, 0xff) / 255.0f;
			Rval.Roughness = 0.15f;
			Rval.Metallic = 0.04f;
		}break;
		case PLASTIC_GREEN: {
			Rval.Color = Vector4f(0x00, 0xff, 0x00, 0xff) / 255.0f;
			Rval.Roughness = 0.15f;
			Rval.Metallic = 0.04f;
		}break;
		case PLASTIC_BLUE: {
			Rval.Color = Vector4f(0x00, 0x00, 0xff, 0xff) / 255.0f;
			Rval.Roughness = 0.15f;
			Rval.Metallic = 0.04f;
		}break;
		case PLASTIC_YELLOW: {
			Rval.Color = Vector4f(0xff, 0xe4, 0x49, 0xff) / 255.0f;
			Rval.Roughness = 0.15f;
			Rval.Metallic = 0.04f;
		}break;

		case STONE_WHITE: {
			Rval.Color = Vector4f(0xff, 0xff, 0xff, 0xff) / 255.0f;
			Rval.Roughness = 0.8f;
			Rval.Metallic = 0.04f;
		}break;
		case STONE_GRAY: {
			Rval.Color = Vector4f(0x90, 0x90, 0x90, 0xff) / 255.0f;
			Rval.Roughness = 0.8f;
			Rval.Metallic = 0.04f;
		}break;
		case STONE_BLACK: {
			Rval.Color = Vector4f(0x00, 0x00, 0x00, 0xff) / 255.0f;
			Rval.Roughness = 0.8f;
			Rval.Metallic = 0.04f;
		}break;
		case STONE_RED: {
			Rval.Color = Vector4f(0xff, 0x00, 0x00, 0xff) / 255.0f;
			Rval.Roughness = 0.8f;
			Rval.Metallic = 0.04f;
		}break;
		case STONE_GREEN: {
			Rval.Color = Vector4f(0x00, 0xff, 0x00, 0xff) / 255.0f;
			Rval.Roughness = 0.8f;
			Rval.Metallic = 0.04f;
		}break;
		case STONE_BLUE: {
			Rval.Color = Vector4f(0x00, 0x00, 0xff, 0xff) / 255.0f;
			Rval.Roughness = 0.8f;
			Rval.Metallic = 0.04f;
		}break;
		case STONE_YELLOW: {
			Rval.Color = Vector4f(0xff, 0xe4, 0x49, 0xff) / 255.0f;
			Rval.Roughness = 0.8f;
			Rval.Metallic = 0.04f;
		}break;
		default: {
			Rval.Color = Vector4f(0xff, 0xff, 0xff, 0xff) / 255.0f;
			Rval.Roughness = 0.8f;
			Rval.Metallic = 0.04f;
		};
		}//switch[Mat]

		return Rval;

	}//retrieveMaterialDefinition

	std::string CForgeUtility::convertToString(const std::u32string String) {
		std::string Rval;
		char C;
		for (auto i : String) {
			std::c32rtomb(&C, i, nullptr);
			Rval.push_back(C);
		}
		return Rval;
	}//convertToString

	std::string CForgeUtility::convertToString(const std::wstring String) {
		std::string Rval;
		char C;
		for (auto i : String) {
			std::c16rtomb(&C, i, nullptr);
			Rval.push_back(C);
		}
		return Rval;
	}//convertToString

	std::u32string CForgeUtility::convertTou32String(const std::string String) {
		std::u32string UString;
		char32_t C;
		for (auto i : String) {
			if(0 <= std::mbrtoc32(&C, &i, 1, nullptr)) UString.push_back(C);
		}
		return UString;
	}//convertTou32String

	std::wstring CForgeUtility::convertToWString(const std::string String) {
		std::wstring Rval;
		char16_t C;
		for (auto i : String) {
			if (0 <= std::mbrtoc16(&C, &i, 1, nullptr)) Rval.push_back(C);
		}
		return Rval;
	}//convertToWString

	Font* CForgeUtility::defaultFont(DefaultFontType FontType, uint32_t FontSize, bool Bold, bool Italic, std::u32string CharSet) {

		Font::FontStyle Style;
		switch (FontType) {
		case FONTTYPE_SERIF: {
			if (Bold && Italic) Style.FileName = "Assets/Fonts/NotoSerif/NotoSerif-BoldItalic.ttf";
			else if (Bold) Style.FileName = "Assets/Fonts/NotoSerif/NotoSerif-Bold.ttf";
			else if (Italic) Style.FileName = "Assets/Fonts/NotoSerif/NotoSerif-Italic.ttf";
			else Style.FileName = "Assets/Fonts/NotoSerif/NotoSerif-Regular.ttf";
		}break;
		case FONTTYPE_SANSERIF: {
			if (Bold && Italic) Style.FileName = "Assets/Fonts/SourceSansPro/SourceSansPro-SemiBoldItalic.ttf";
			else if (Bold) Style.FileName = "Assets/Fonts/SourceSansPro/SourceSansPro-SemiBold.ttf";
			else if (Italic) Style.FileName = "Assets/Fonts/SourceSansPro/SourceSansPro-Italic.ttf";
			else Style.FileName = "Assets/Fonts/SourceSansPro/SourceSansPro-Regular.ttf";
		}break;
		case FONTTYPE_MONO: {
			if (Bold && Italic) Style.FileName = "Assets/Fonts/UbuntuMono/UbuntuMono-BoldItalic.ttf";
			else if (Bold) Style.FileName = "Assets/Fonts/UbuntuMono/UbuntuMono-Bold.ttf";
			else if (Italic) Style.FileName = "Assets/Fonts/UbuntuMono/UbuntuMono-Italic.ttf";
			else Style.FileName = "Assets/Fonts/UbuntuMono/UbuntuMono-Regular.ttf";
		}break;
		case FONTTYPE_HANDWRITING: {
			// for handwriting we have to varying type faces
			Style.FileName = "Assets/Fonts/RougeScript/RougeScript-Regular.ttf";
		}break;
		default: throw CForgeExcept("Unknown font type specified!");
		}

		Style.PixelSize = FontSize;
		if (!CharSet.empty()) Style.CharacterSet = CharSet;

		FontManager* pFMan = FontManager::instance();
		Font* pRval = pFMan->createFont(Style);
		pFMan->release();
		return pRval;
	}//defaultFont

}//name space