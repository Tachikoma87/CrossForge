#include <glad/glad.h>
#include "GLTexture2D.h"

namespace CForge {
	GLTexture2D::GLTexture2D(void): CForgeObject("GLTexture2D") {
		m_TexObj = GL_INVALID_INDEX;
	}//Constructor

	GLTexture2D::~GLTexture2D(void) {
		clear();
	}//Destructor

	void GLTexture2D::init(const T2DImage<uint8_t>* pImage, bool GenerateMipmaps) {
		if (nullptr == pImage) throw NullpointerExcept("pImage");
		if (pImage->width() == 0 || pImage->height() == 0) throw CForgeExcept("Image contains no data!");
		clear();

		// generate texture
		glGenTextures(1, &m_TexObj);
		glBindTexture(GL_TEXTURE_2D, m_TexObj);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pImage->width(), pImage->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pImage->data());

		if (GenerateMipmaps) {
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
		
		
	}//initialize

	void GLTexture2D::clear(void) {
		if (glIsTexture(m_TexObj)) {
			glDeleteTextures(1, &m_TexObj);
			m_TexObj = GL_INVALID_INDEX;
		}
	}//clear

	void GLTexture2D::bind(void) {
		if (GL_INVALID_INDEX == m_TexObj) throw CForgeExcept("Texture object is invalid!");
		glBindTexture(GL_TEXTURE_2D, m_TexObj);
	}//bind

	uint32_t GLTexture2D::handle(void)const {
		return m_TexObj;
	}//handle

}//name space