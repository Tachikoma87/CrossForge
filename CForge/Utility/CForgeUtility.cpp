#include <glad/glad.h>
#include "CForgeUtility.h"
#include "../Math/CForgeMath.h"

namespace CForge {

	CForgeUtility::CForgeUtility(void): CForgeObject("CForgeUtility") {

	}//Constructor

	CForgeUtility::~CForgeUtility(void) {

	}//Destructor

	void CForgeUtility::retrieveColorTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg) {
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		if (!glIsTexture(TexObj)) throw CForgeExcept("Specified object is not a valid OpenGL texture.");
		glBindTexture(GL_TEXTURE_2D, TexObj);

		int32_t TexWidth = 0;
		int32_t TexHeight = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &TexWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &TexHeight);

		uint8_t* pBuffer = new uint8_t[TexWidth * TexHeight * 3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);

		pImg->init(TexWidth, TexHeight, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);

		delete[] pBuffer;
	}//retrieveDeptBuffer

	void CForgeUtility::retrieveDepthTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg, float Near, float Far) {
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		if (!glIsTexture(TexObj)) throw CForgeExcept("Specified object is not a valid OpenGL texture.");
		glBindTexture(GL_TEXTURE_2D, TexObj);
		int32_t TexWidth = 0;
		int32_t TexHeight = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &TexWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &TexHeight);

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

		delete[] pBuffer;
		delete[] pDepthBuffer;
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

		int32_t TotalMemory = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &TotalMemory);
		return TotalMemory;
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

			delete[] pBuffer;
			delete[] pDepthBuffer;
		}

	}//fetchFramebuffer

	CForgeUtility::GPUTraits CForgeUtility::retrieveGPUTraits(void) {
		GPUTraits Rval;

		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Rval.MaxTextureImageUnits);

		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &Rval.MaxFragmentUniformComponents);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &Rval.MaxFragmentUniformBLocks);
		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &Rval.MaxFramebufferWidth);
		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &Rval.MaxFramebufferHeight);

		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &Rval.MaxUniformBlockSize);

		glGetIntegerv(GL_MAX_VARYING_VECTORS, &Rval.MaxVaryingVectors);

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &Rval.MaxVertexAttribs);

		GLint MinorVersion = 0;
		GLint MajorVersion = 0;
		glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);
		glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
		Rval.GLMinorVersion = MinorVersion;
		Rval.GLMajorVersion = MajorVersion;

		Rval.GLVersion = (const char*)glGetString(GL_VERSION);

		return Rval;
	}//retrieveGPUTraits

}//name space