#include "OpenGLHeader.h"
#include "../Core/SLogger.h"
#include "GBuffer.h"
#include "../Utility/CForgeUtility.h"

namespace CForge {
	GBuffer::GBuffer(void): CForgeObject("GBuffer") {
		m_Framebuffer = GL_INVALID_INDEX;
		m_Renderbuffer = GL_INVALID_INDEX;
		m_TexPosition = GL_INVALID_INDEX;
		m_TexNormal = GL_INVALID_INDEX;
		m_TexAlbedo = GL_INVALID_INDEX;
		m_TexDepthStencil = GL_INVALID_INDEX;
		m_Width = 0;
		m_Height = 0;
	}//Constructor

	GBuffer::~GBuffer(void) {
		clear();
	}//Destructor


	void GBuffer::init(uint32_t Width, uint32_t Height) {
		if (Width == 0 || Height == 0) {//TODO gets thrown when window is minimized, make window size 1x1px instead
			Width = 1; Height = 1;
		}
			//throw CForgeExcept("Zero width or height for GBuffer specified!");
		
		std::string ErrorMsg;
		if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			SLogger::log("Not handled OpenGL error occurred before initialization of GBuffer: " + ErrorMsg, "GBuffer", SLogger::LOGTYPE_ERROR);
		}


		// multisample not finished implementing, not working with deferred shading right now as it requires new shaders using sampler2DMS
		const int32_t Multisample = 0;

		clear();

		m_Width = Width;
		m_Height = Height;

		if (nullptr == glGenFramebuffers) throw CForgeExcept("Framebuffers not available!");
		if (nullptr == glGenRenderbuffers) throw CForgeExcept("Renderbuffers not available!");

		// generate framebuffer
		glGenFramebuffers(1, &m_Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

		// create texture for position data
		glGenTextures(1, &m_TexPosition);
		// create texture for normal data
		glGenTextures(1, &m_TexNormal);
		// create texture for albedo/specular data
		glGenTextures(1, &m_TexAlbedo);
		// create texture for depth and stencil
		glGenTextures(1, &m_TexDepthStencil);


		if (Multisample > 0) {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_TexPosition);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Multisample, GL_RGBA, m_Width, m_Height, GL_TRUE);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_TexPosition, 0);

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_TexNormal);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Multisample, GL_RGBA, m_Width, m_Height, GL_TRUE);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, m_TexNormal, 0);

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_TexAlbedo);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Multisample, GL_RGBA, m_Width, m_Height, GL_TRUE);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, m_TexAlbedo, 0);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, m_TexPosition);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexPosition, 0);

			glBindTexture(GL_TEXTURE_2D, m_TexNormal);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_TexNormal, 0);

			
			glBindTexture(GL_TEXTURE_2D, m_TexAlbedo);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_TexAlbedo, 0);	
		}

		// and now the depth and stencil attachment
		glBindTexture(GL_TEXTURE_2D, m_TexDepthStencil);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_TexDepthStencil, 0);
		
		uint32_t Attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, Attachments);

		// generate Renerbuffer
		glGenRenderbuffers(1, &m_Renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_Renderbuffer);

		if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
			std::string Error;
			CForgeUtility::checkGLError(&Error);
			throw CForgeExcept("Generating framebuffer for gBuffer failed!\n\t" + Error);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			SLogger::log("Not handled OpenGL error occurred during initialization of GBuffer: " + ErrorMsg, "GBuffer", SLogger::LOGTYPE_ERROR);
		}

	}//initialize

	void GBuffer::clear(void) {
		// delete renderbuffer and renderbuffer
		if (glIsFramebuffer(m_Framebuffer)) glDeleteFramebuffers(1, &m_Framebuffer);
		if (glIsRenderbuffer(m_Renderbuffer)) glDeleteRenderbuffers(1, &m_Renderbuffer);

		// clear textures
		if (glIsTexture(m_TexAlbedo)) glDeleteTextures(1, &m_TexAlbedo);
		if (glIsTexture(m_TexNormal)) glDeleteTextures(1, &m_TexNormal);
		if (glIsTexture(m_TexPosition)) glDeleteTextures(1, &m_TexPosition);
		if (glIsTexture(m_TexDepthStencil)) glDeleteTextures(1, &m_TexDepthStencil);

		m_Framebuffer = GL_INVALID_INDEX;
		m_Renderbuffer = GL_INVALID_INDEX;
		m_TexAlbedo = GL_INVALID_INDEX;
		m_TexNormal = GL_INVALID_INDEX;
		m_TexPosition = GL_INVALID_INDEX;
		m_TexDepthStencil = GL_INVALID_INDEX;

		m_Width = 0;
		m_Height = 0;
	}//clear

	void GBuffer::bind(void)const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
	}//bind

	void GBuffer::unbind(void)const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}//unbind

	uint32_t GBuffer::width(void)const {
		return m_Width;
	}//width

	uint32_t GBuffer::height(void)const {
		return m_Height;
	}//height

	void GBuffer::bindTexture(Component Comp, uint32_t Level) {
		glActiveTexture(GL_TEXTURE0 + Level);
		switch (Comp) {
		case COMP_POSITION: {
			glBindTexture(GL_TEXTURE_2D, m_TexPosition);
		}break;
		case COMP_NORMAL: {
			glBindTexture(GL_TEXTURE_2D, m_TexNormal);
		}break;
		case COMP_ALBEDO: {
			glBindTexture(GL_TEXTURE_2D, m_TexAlbedo);
		}break;
		case COMP_DEPTH_STENCIL: {
			glBindTexture(GL_TEXTURE_2D, m_TexDepthStencil);
		}break;
		default: throw CForgeExcept("Invalid gBuffer component specified!");
		}
	}//bindTexture

	void GBuffer::blitDepthBuffer(uint32_t Width, uint32_t Height) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffer);
		glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}//blitDeptHBuffer

	void GBuffer::blitDepthBuffer(Eigen::Vector2i SourcePos, Eigen::Vector2i SourceSize, Eigen::Vector2i TargetPos, Eigen::Vector2i TargetSize) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffer);
		glBlitFramebuffer(
			SourcePos.x(), SourcePos.y(), SourcePos.x() + SourceSize.x(), SourcePos.y() + SourceSize.y(),
			TargetPos.x(), TargetPos.y(), TargetPos.x() + TargetSize.x(), TargetPos.y() + TargetSize.y(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void GBuffer::retrievePositionBuffer(T2DImage<uint8_t>* pImg){
#ifndef __EMSCRIPTEN__
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		glBindTexture(GL_TEXTURE_2D, m_TexPosition);
		uint8_t *pBuffer = new uint8_t[m_Width * m_Height * 3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);
		pImg->init(m_Width, m_Height, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
		delete[] pBuffer;
#endif
	}//readPosition

	void GBuffer::retrieveNormalBuffer(T2DImage<uint8_t>* pImg) {
#ifndef __EMSCRIPTEN__
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		glBindTexture(GL_TEXTURE_2D, m_TexNormal);
		uint8_t* pBuffer = new uint8_t[m_Width * m_Height * 3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);
		pImg->init(m_Width, m_Height, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
		delete[] pBuffer;
#endif
	}//retrieveNormalBuffer

	void GBuffer::retrieveAlbedoBuffer(T2DImage<uint8_t>* pImg) {
#ifndef __EMSCRIPTEN__
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		glBindTexture(GL_TEXTURE_2D, m_TexAlbedo);
		uint8_t* pBuffer = new uint8_t[m_Width * m_Height * 3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);
		pImg->init(m_Width, m_Height, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
		delete[] pBuffer;
#endif
	}//retrieveAlbedoBuffer

	void GBuffer::retrieveDepthBuffer(T2DImage<uint8_t>* pImg, float Near, float Far) {
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		CForgeUtility::retrieveDepthTexture(m_TexDepthStencil, pImg, Near, Far);
	}//retrieveDepthBuffer

}//name space