#include "../OpenGLHeader.h"
#include "GLTexture2D.h"

namespace CForge {
	GLTexture2D::GLTexture2D(void): CForgeObject("GLTexture2D") {
		m_TexObj = GL_INVALID_INDEX;
		m_Width = 0;
		m_Height = 0;
	}//Constructor

    GLTexture2D::GLTexture2D(uint32_t handle) : CForgeObject("GLTexture2D"), m_TexObj(handle) {}

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

		T2DImage<uint8_t> Img;
		Img.init(pImage->width(), pImage->height(), pImage->colorSpace(), pImage->data());
		Img.flipRows();

		uint32_t ColorSpace = 0;
		switch (Img.colorSpace()) {
		case T2DImage<uint8_t>::COLORSPACE_GRAYSCALE: ColorSpace = GL_R; break;
		case T2DImage<uint8_t>::COLORSPACE_RGB: ColorSpace = GL_RGB; break;
		case T2DImage<uint8_t>::COLORSPACE_RGBA: ColorSpace = GL_RGBA; break;
		default: throw CForgeExcept("Image has unknown color space and can not be used as a texture!"); break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, ColorSpace, Img.width(), Img.height(), 0, ColorSpace, GL_UNSIGNED_BYTE, Img.data());

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
		
		m_Width = Img.width();
		m_Height = Img.height();
	}//initialize

	void GLTexture2D::clear(void) {
		if (glIsTexture(m_TexObj)) {
			glDeleteTextures(1, &m_TexObj);
			m_TexObj = GL_INVALID_INDEX;
		}
		m_Width = 0;
		m_Height = 0;
	}//clear

	void GLTexture2D::bind(void) {
		if (GL_INVALID_INDEX == m_TexObj) throw CForgeExcept("Texture object is invalid!");
		glBindTexture(GL_TEXTURE_2D, m_TexObj);
	}//bind

	void GLTexture2D::unbind(void) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}//unbind

	uint32_t GLTexture2D::handle(void)const {
		return m_TexObj;
	}//handle

	int32_t GLTexture2D::width(void)const {
		return m_Width;
	}//width

	int32_t GLTexture2D::height(void)const {
		return m_Height;
	}//height

}//name space