#include <CForge/Graphics/OpenGLHeader.h>
#include "GLRenderbuffer.h"
#include <CForge/Utility/CForgeUtility.h>

namespace CForge {

	GLRenderbuffer::GLRenderbuffer(void) : CForgeObject("ForwardBuffer") {
		m_Framebuffer = GL_INVALID_INDEX;
		m_Renderbuffer = GL_INVALID_INDEX;
		m_TexColor = GL_INVALID_INDEX;
		m_TexDepthStencil = GL_INVALID_INDEX;
		m_Width = 0;
		m_Height = 0;
	}//Constructor

	GLRenderbuffer::~GLRenderbuffer(void) {
		clear();
	}//Destructor

	void GLRenderbuffer::init(uint32_t Width, uint32_t Height) {
		if (Width == 0 || Height == 0) throw CForgeExcept("Invalid dimensions defined for forward buffer!");
		clear();

		if (nullptr == glGenFramebuffers) throw CForgeExcept("Framebuffers not available!");
		if (nullptr == glGenRenderbuffers) throw CForgeExcept("Renderbuffers not available!");

		m_Width = Width;
		m_Height = Height;

		// generate framebuffer
		glGenFramebuffers(1, &m_Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

		// generate color  and depth/stencil texture
		glGenTextures(1, &m_TexColor);
		glGenTextures(1, &m_TexDepthStencil);

		// generate color texture
		glBindTexture(GL_TEXTURE_2D, m_TexColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexColor, 0);

		// build depth/stencil texture
		glBindTexture(GL_TEXTURE_2D, m_TexDepthStencil);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_TexDepthStencil, 0);

		// tell OpenGL where we are drawing
		uint32_t Attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, Attachments);

		// generate Renerbuffer
		glGenRenderbuffers(1, &m_Renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_Renderbuffer);

		if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
			std::string Error;
			CForgeUtility::checkGLError(&Error);
			throw CForgeExcept("Generating framebuffer for gBuffer failed!\n\t" + Error);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}//initialize

	void GLRenderbuffer::clear(void) {
		// delete renderbuffer and renderbuffer
		if (glIsFramebuffer(m_Framebuffer)) glDeleteFramebuffers(1, &m_Framebuffer);
		if (glIsRenderbuffer(m_Renderbuffer)) glDeleteRenderbuffers(1, &m_Renderbuffer);

		// clear textures
		if (glIsTexture(m_TexColor)) glDeleteTextures(1, &m_TexColor);
		if (glIsTexture(m_TexDepthStencil)) glDeleteTextures(1, &m_TexDepthStencil);


		m_Framebuffer = GL_INVALID_INDEX;
		m_Renderbuffer = GL_INVALID_INDEX;
		m_TexColor = GL_INVALID_INDEX;
		m_TexDepthStencil = GL_INVALID_INDEX;

		m_Width = 0;
		m_Height = 0;
	}//clear

	void GLRenderbuffer::bind(void)const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
	}//bind

	void GLRenderbuffer::unbind(void)const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}//unbind

	void GLRenderbuffer::bindTexture(Component Comp, uint32_t Level) {
		glActiveTexture(GL_TEXTURE0 + Level);
		switch (Comp) {
		case COMP_COLOR: {
			glBindTexture(GL_TEXTURE_2D, m_TexColor);
		}break;
		case COMP_DEPTH_STENCIL: {
			glBindTexture(GL_TEXTURE_2D, m_TexDepthStencil);
		}break;
		default: throw CForgeExcept("Invalid forward buffer component specified!");
		}
	}//bindTexture

	void GLRenderbuffer::blitDepthBuffer(uint32_t Width, uint32_t Height) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffer);
		glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}//blitDepthBuffer

	uint32_t GLRenderbuffer::width(void)const {
		return m_Width;
	}//width

	uint32_t GLRenderbuffer::height(void)const {
		return m_Height;
	}//height

	void GLRenderbuffer::retrieveColorBuffer(T2DImage<uint8_t>* pImg) {
		CForgeUtility::retrieveColorTexture(m_TexColor, pImg);
	}//retrieve

	void GLRenderbuffer::retrieveDepthBuffer(T2DImage<uint8_t>* pImg, float Near, float Far) {
		CForgeUtility::retrieveDepthTexture(m_TexDepthStencil, pImg, Near, Far);
	}//retrieveDepthBuffer

}//name space