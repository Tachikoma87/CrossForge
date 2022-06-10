#include <glad/glad.h>
#include "GraphicsUtility.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	Eigen::Matrix4f GraphicsUtility::perspectiveProjection(uint32_t Width, uint32_t Height, float FieldOfView, float Near, float Far) {
		if (Near >= Far) throw CForgeExcept("Near plane further away than far plane!");
		if (Width == 0 || Height == 0) throw CForgeExcept("Viewport width and/or viewport height specified with 0!");

	
		Eigen::Matrix4f Rval = Eigen::Matrix4f::Identity();

		float Aspect = float(Width) / float(Height);
		float Theta = FieldOfView * 0.5f;
		float Range = Far - Near;
		float InvTan = 1.0 / std::tan(Theta);

		Rval(0, 0) = InvTan / Aspect;
		Rval(1, 1) = InvTan;
		Rval(2, 2) = -(Near + Far) / Range;
		Rval(3, 2) = -1.0f;
		Rval(2, 3) = -2.0f * Near * Far / Range;
		Rval(3, 3) = 0.0f;

		return Rval;
	}//perspective

	Eigen::Matrix4f GraphicsUtility::orthographicProjection(float Left, float Right, float Bottom, float Top, float Near, float Far) {
		Eigen::Matrix4f Rval = Eigen::Matrix4f::Identity();
		Rval(0, 0) = 2.0f / (Right - Left);
		Rval(0, 3) = -(Right + Left) / (Right - Left);
		Rval(1, 1) = 2.0f / (Top - Bottom);
		Rval(1, 3) = -(Top + Bottom) / (Top - Bottom);
		Rval(2, 2) = -2.0f / (Far - Near);
		Rval(2, 3) = -(Far + Near) / (Far - Near);
		
		return Rval;
	}//orthographic

	Eigen::Matrix4f GraphicsUtility::orthographicProjection(float Right, float Top, float Near, float Far) {
		Eigen::Matrix4f Rval = Eigen::Matrix4f::Identity();

		Rval(0, 0) = 1.0f / Right;
		Rval(1, 1) = 1.0f / Top;
		Rval(2, 2) = -2.0f / (Far - Near);
		Rval(2, 3) = -(Far + Near) / (Far - Near);

		return Rval;
	}//orthographic

	Eigen::Matrix4f GraphicsUtility::lookAt(Eigen::Vector3f Position, Eigen::Vector3f Target, Eigen::Vector3f Up) {
		Eigen::Matrix4f Rval = Eigen::Matrix4f::Identity();

		Eigen::Vector3f f = Target - Position;

		Eigen::Vector3f s = f.cross(Up);
		Eigen::Vector3f u = s.cross(f);

		f.normalize();
		s.normalize();
		u.normalize();

		Rval(0, 0) = s.x();
		Rval(0, 1) = s.y();
		Rval(0, 2) = s.z();
		Rval(1, 0) = u.x();
		Rval(1, 1) = u.y();
		Rval(1, 2) = u.z();
		Rval(2, 0) = -f.x();
		Rval(2, 1) = -f.y();
		Rval(2, 2) = -f.z();

		Eigen::Vector4f V = Rval * Eigen::Vector4f(-Position.x(), -Position.y(), -Position.z(), 1.0f);
		Rval(0, 3) = V.x();
		Rval(1, 3) = V.y();
		Rval(2, 3) = V.z();

		return Rval;
	}//lookAt

	void GraphicsUtility::retrieveColorTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg) {
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

	void GraphicsUtility::retrieveDepthTexture(uint32_t TexObj, T2DImage<uint8_t>* pImg, float Near, float Far) {
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		if (!glIsTexture(TexObj)) throw CForgeExcept("Specified object is not a valid OpenGL texture.");
		glBindTexture(GL_TEXTURE_2D, TexObj);
		int32_t TexWidth = 0;
		int32_t TexHeight = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &TexWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &TexHeight);

		float* pDepthBuffer = new float[TexWidth*TexHeight];
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
		for (uint32_t i = 0; i <TexWidth * TexHeight; ++i) {
			pBuffer[i * 3 + 0] = pDepthBuffer[i] * 255.0f;
			pBuffer[i * 3 + 1] = pDepthBuffer[i] * 255.0f;
			pBuffer[i * 3 + 2] = pDepthBuffer[i] * 255.0f;
		}
		pImg->init(TexWidth, TexHeight, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);

		delete[] pBuffer;
		delete[] pDepthBuffer;
	}//retrieveDeptBuffer

	
	uint32_t GraphicsUtility::checkGLError(std::string *pVerbose) {
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
			case GL_INVALID_FRAMEBUFFER_OPERATION:{
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

		return (GL_NO_ERROR != Rval);
	}//checkGLError

	uint32_t GraphicsUtility::gpuMemoryAvailable(void) {
		const uint32_t GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX = 0x9048;

		int32_t TotalMemory = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &TotalMemory);
		return TotalMemory;
	}//gpuMemoryAvailable

	uint32_t GraphicsUtility::gpuFreeMemory(void) {
		const uint32_t GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX = 0x9049;
		int32_t AvailableMemory = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &AvailableMemory);
		return AvailableMemory;
	}//gpuMemoryUsage

	Eigen::Matrix4f GraphicsUtility::rotationMatrix(Eigen::Quaternionf Rot) {
		Matrix4f Rval = Matrix4f::Identity();
		const Matrix3f R = Rot.toRotationMatrix();
		for (uint8_t i = 0; i < 3; ++i) {
			for (uint8_t k = 0; k < 3; ++k) {
				Rval(i, k) = R(i, k);
			}
		}
		return Rval;
	}//rotationMatrix

	Eigen::Matrix4f GraphicsUtility::translationMatrix(Eigen::Vector3f Trans) {
		Matrix4f Rval = Matrix4f::Identity();
		Rval(0, 3) = Trans.x();
		Rval(1, 3) = Trans.y();
		Rval(2, 3) = Trans.z();
		return Rval;
	}//translationMatrix

	Eigen::Matrix4f GraphicsUtility::scaleMatrix(Eigen::Vector3f Scale) {
		Matrix4f Rval = Matrix4f::Identity();
		Rval(0, 0) = Scale.x();
		Rval(1, 1) = Scale.y();
		Rval(2, 2) = Scale.z();
		Rval(3, 3) = 1.0f;
		return Rval;
	}//scaleMatrix

	// thanks to: https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
	Eigen::Matrix3f GraphicsUtility::alignVectors(const Eigen::Vector3f Source, const Eigen::Vector3f Target) {
		Vector3f a = Source;
		Vector3f b = Target;

		Vector3f v = a.cross(b);
		float s = v.norm();
		float c = a.dot(b);

		Matrix3f vx;
		vx(0, 0) = 0.0f;
		vx(0, 1) = -v.z();
		vx(0, 2) = v.y();

		vx(1, 0) = v.z();
		vx(1, 1) = 0.0f;
		vx(1, 2) = -v.x();

		vx(2, 0) = -v.y();
		vx(2, 1) = v.x();
		vx(2, 2) = 0.0f;

		Matrix3f Rval = Matrix3f::Identity() + vx + (1.0f - c) / (s * s) * vx * vx;
		return Rval;
	}//alignVectors

	void GraphicsUtility::retrieveFrameBuffer(T2DImage<uint8_t>* pColor, T2DImage<uint8_t>* pDepth, float Near, float Far) {

		// get framebuffer width and height
		int32_t Viewport[4];
		glGetIntegerv(GL_VIEWPORT, Viewport);
		
		int32_t x = Viewport[0];
		int32_t y = Viewport[1];
		int32_t Width = Viewport[2] - Viewport[0];
		int32_t Height = Viewport[3] - Viewport[1];

		if (nullptr != pColor) {
			uint8_t* pBuffer = new uint8_t[Width * Height * 3];
			glReadPixels(x, y, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);
			pColor->init(Width, Height, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
			delete[] pBuffer;
		}

		if (nullptr != pDepth) {
			float* pBuffer = new float[Width * Height];
			uint8_t *pDepthBuffer = new uint8_t[Width * Height];
			glReadPixels(x, y, Width, Height, GL_DEPTH_COMPONENT, GL_FLOAT, pBuffer);

			// linearize depth?
			if (Near > 0.0f && Far > 0.0f) {
				for (uint32_t i = 0; i < Width * Height; ++i) {
					float z = pBuffer[i] * 2.0f - 1.0f; // back to NDC
					pBuffer[i] = (2.0f * Near * Far) / ( Far + Near - z * (Far - Near));
					pBuffer[i] /= (Far - Near);
				}//For[all depth values]
			}//if[linearize depth values]

			for (uint32_t i = 0; i < Width * Height; ++i) pDepthBuffer[i] = (uint8_t)(pBuffer[i] * 255.0f);
			pDepth->init(Width, Height, T2DImage<uint8_t>::COLORSPACE_GRAYSCALE, pDepthBuffer);

			delete[] pBuffer;
			delete[] pDepthBuffer;
		}
		
	}//fetchFramebuffer

	GraphicsUtility::GPUTraits GraphicsUtility::retrieveGPUTraits(void) {
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


	GraphicsUtility::GraphicsUtility(void): CForgeObject("GraphicsUtility") {

	}//Constructor

	GraphicsUtility::~GraphicsUtility(void) {

	}//Destructor

	void GraphicsUtility::init(void) {

	}//initialize

	void GraphicsUtility::clear(void) {

	}//clear

}//name space
