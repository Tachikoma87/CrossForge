#include "OpenGLHeader.h"
#include "GLVertexArray.h"

namespace CForge {

	bool GLVertexArray::available(void) {
		return (glGenVertexArrays != nullptr);
	}//available

	GLVertexArray::GLVertexArray(void): CForgeObject("GLVertexArray") {
		m_GLID = GL_INVALID_INDEX;
	}//Constructor

	GLVertexArray::~GLVertexArray(void) {
		clear();
	}//Destructor

	void GLVertexArray::init(void) {
		if (!available()) throw CForgeExcept("Vertex arrays not available!");
		clear();
		glGenVertexArrays(1, &m_GLID);
	}//initialize

	void GLVertexArray::clear(void) {
		if (glIsVertexArray(m_GLID)) glDeleteVertexArrays(1, &m_GLID);
		m_GLID = GL_INVALID_INDEX;
	}//clear

	void GLVertexArray::bind(void) {
		glBindVertexArray(m_GLID);
	}//bind

	void GLVertexArray::unbind(void) {
		glBindVertexArray(0);
	}//unbind

}//name space