#include <glad/glad.h>
#include "GraphicsUtility.h"

namespace crossforge {

	uint32_t GraphicsUtility::checkGLError(std::string* pVerbose) {
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


	GraphicsUtility::GraphicsUtility() {

	}
	GraphicsUtility::~GraphicsUtility() {

	}
}