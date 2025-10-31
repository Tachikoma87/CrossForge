#include "../OpenGLHeader.h"
#include "GLCubemap.h"

namespace CForge {
	GLCubemap::GLCubemap(void): CForgeObject("GLCubemap") {
		m_TexObj = GL_INVALID_INDEX;
	}//Constructor

	GLCubemap::~GLCubemap(void) {
		clear();
	}//Destructor

	void GLCubemap::init(const T2DImage<uint8_t>* pRight, const T2DImage<uint8_t>* pLeft, const T2DImage<uint8_t>* pTop,
		const T2DImage<uint8_t>* pBottom, T2DImage<uint8_t>* pBack, T2DImage<uint8_t>* pFront) {

		if (nullptr == pRight) throw NullpointerExcept("pRight");
		if (nullptr == pLeft)throw NullpointerExcept("pLeft");
		if (nullptr == pTop)throw NullpointerExcept("pTop");
		if (nullptr == pBottom) throw NullpointerExcept("pBottom");
		if (nullptr == pBack) throw NullpointerExcept("pBack");
		if (nullptr == pFront) throw NullpointerExcept("pFront");

		clear();

		glGenTextures(1, &m_TexObj);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TexObj);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, pRight->width(), pRight->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pRight->data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, pLeft->width(), pLeft->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pLeft->data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, pTop->width(), pTop->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pTop->data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, pBottom->width(), pBottom->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pBottom->data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, pBack->width(), pBack->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pBack->data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, pFront->width(), pFront->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pFront->data());

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}//initialize

	void GLCubemap::clear(void) {
		if (glIsTexture(m_TexObj)) glDeleteTextures(1, &m_TexObj);
		m_TexObj = GL_INVALID_INDEX;
	}//clear


	void GLCubemap::bind(void) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TexObj);
	}//bind

	void GLCubemap::unbind(void) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}//unbind

	uint32_t GLCubemap::handle(void)const {
		return m_TexObj;
	}//handle

}//name space