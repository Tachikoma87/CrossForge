#include <glad/glad.h>
#include "GBuffer.h"

namespace CForge {
	GBuffer::GBuffer(void): CForgeObject("GBuffer") {
		m_Framebuffer = GL_INVALID_INDEX;
		m_Renderbuffer = GL_INVALID_INDEX;
		m_TexPosition = GL_INVALID_INDEX;
		m_TexNormal = GL_INVALID_INDEX;
		m_TexAlbedo = GL_INVALID_INDEX;
		m_Width = 0;
		m_Height = 0;
	}//Constructor

	GBuffer::~GBuffer(void) {
		clear();
	}//Destructor

	void GBuffer::init(uint32_t Width, uint32_t Height) {
		if (Width == 0 || Height == 0) throw CForgeExcept("Zero width or height for GBuffer specified!");
		
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
		glBindTexture(GL_TEXTURE_2D, m_TexPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexPosition, 0);

		// create texture for normal data
		glGenTextures(1, &m_TexNormal);
		glBindTexture(GL_TEXTURE_2D, m_TexNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_TexNormal, 0);

		// create texture for albedo/specular data
		glGenTextures(1, &m_TexAlbedo);
		glBindTexture(GL_TEXTURE_2D, m_TexAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_TexAlbedo, 0);

		uint32_t Attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, Attachments);
		
		// create texture for depth/stencil data
		glGenTextures(1, &m_TexDepth);
		glBindTexture(GL_TEXTURE_2D, m_TexDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TexDepth, 0);


		/*
		// generate Renerbuffer
		glGenRenderbuffers(1, &m_Renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_Renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer);
		*/


		//if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) throw CForgeExcept("Generating framebuffer for gBuffer failed!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}//initialize

	void GBuffer::clear(void) {
		// delete renderbuffer and renderbuffer
		if (glIsFramebuffer(m_Framebuffer)) glDeleteFramebuffers(1, &m_Framebuffer);
		if (glIsRenderbuffer(m_Renderbuffer)) glDeleteRenderbuffers(1, &m_Renderbuffer);

		// clear textures
		if (glIsTexture(m_TexAlbedo)) glDeleteTextures(1, &m_TexAlbedo);
		if (glIsTexture(m_TexNormal)) glDeleteTextures(1, &m_TexNormal);
		if (glIsTexture(m_TexPosition)) glDeleteTextures(1, &m_TexPosition);

		m_Framebuffer = GL_INVALID_INDEX;
		m_Renderbuffer = GL_INVALID_INDEX;
		m_TexAlbedo = GL_INVALID_INDEX;
		m_TexNormal = GL_INVALID_INDEX;
		m_TexPosition = GL_INVALID_INDEX;

		m_Width = 0;
		m_Height = 0;
	}//clear

	void GBuffer::bind(void)const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
		default: throw CForgeExcept("Invalid gBuffer component specified!");
		}
	}//bindTexture

	void GBuffer::blitDepthBuffer(uint32_t Width, uint32_t Height) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffer);
		glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}//blitDeptHBuffer

	void GBuffer::retrievePositionBuffer(T2DImage<uint8_t>* pImg){
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		glBindTexture(GL_TEXTURE_2D, m_TexPosition);
		uint8_t *pBuffer = new uint8_t[m_Width * m_Height * 3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);
		pImg->init(m_Width, m_Height, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
		delete[] pBuffer;
	}//readPosition

	void GBuffer::retrieveNormalBuffer(T2DImage<uint8_t>* pImg) {
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		glBindTexture(GL_TEXTURE_2D, m_TexNormal);
		uint8_t* pBuffer = new uint8_t[m_Width * m_Height * 3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);
		pImg->init(m_Width, m_Height, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
		delete[] pBuffer;
	}//retrieveNormalBuffer

	void GBuffer::retrieveAlbedoBuffer(T2DImage<uint8_t>* pImg) {
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		glBindTexture(GL_TEXTURE_2D, m_TexAlbedo);
		uint8_t* pBuffer = new uint8_t[m_Width * m_Height * 3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);
		pImg->init(m_Width, m_Height, T2DImage<uint8_t>::COLORSPACE_RGB, pBuffer);
		delete[] pBuffer;

	}//retrieveAlbedoBuffer

	uint32_t GBuffer::retrieveFrameBuffer() {
		return m_Framebuffer;
	}

}//name space