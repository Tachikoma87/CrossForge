#include <glad/glad.h>
#include "PPBuffer.h"

namespace CForge {
	PPBuffer::PPBuffer(void): CForgeObject("PPBuffer") {
		m_Framebuffer = GL_INVALID_INDEX;
		m_Renderbuffer = GL_INVALID_INDEX;
		m_Tex = GL_INVALID_INDEX;
		m_Width = 0;
		m_Height = 0;
	}//Constructor

	PPBuffer::~PPBuffer(void) {
		clear();
	}//Destructor

	void PPBuffer::init(uint32_t Width, uint32_t Height) {
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
		glGenTextures(1, &m_Tex);
		glBindTexture(GL_TEXTURE_2D, m_Tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Tex, 0);

		uint32_t Attachments[1] = { GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, Attachments);

		// generate Renerbuffer
		glGenRenderbuffers(1, &m_Renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_Renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer);

		if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) throw CForgeExcept("Generating framebuffer for gBuffer failed!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}//initialize

	void PPBuffer::clear(void) {
		// delete renderbuffer and renderbuffer
		if (glIsFramebuffer(m_Framebuffer)) glDeleteFramebuffers(1, &m_Framebuffer);
		if (glIsRenderbuffer(m_Renderbuffer)) glDeleteRenderbuffers(1, &m_Renderbuffer);

		// clear textures
		if (glIsTexture(m_Tex)) glDeleteTextures(1, &m_Tex);

		m_Framebuffer = GL_INVALID_INDEX;
		m_Renderbuffer = GL_INVALID_INDEX;
		m_Tex = GL_INVALID_INDEX;

		m_Width = 0;
		m_Height = 0;
	}//clear

	void PPBuffer::bind(void)const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}//bind

	void PPBuffer::unbind(void)const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}//unbind

	uint32_t PPBuffer::width(void)const {
		return m_Width;
	}//width

	uint32_t PPBuffer::height(void)const {
		return m_Height;
	}//height

	void PPBuffer::bindTexture(Component Comp, uint32_t Level) {
		glActiveTexture(GL_TEXTURE0 + Level);
		switch (Comp) {
		case COMP: {
			glBindTexture(GL_TEXTURE_2D, m_Tex);
			}break;
		default: throw CForgeExcept("Invalid gBuffer component specified!");
		}
	}//bindTexture

	void PPBuffer::blitDepthBuffer(uint32_t Width, uint32_t Height) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffer);
		glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}//blitDeptHBuffer

	void PPBuffer::blitColorBuffer(uint32_t Width, uint32_t Height) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffer);
		glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}//blitDeptHBuffer

	uint32_t PPBuffer::getFramebufferIndex() {
		return m_Framebuffer;
	}
}//name space