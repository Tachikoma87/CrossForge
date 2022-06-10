#ifdef __OPENGLES__
#define GLFW_INCLUDE_ES3
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Input/SInputManager.h"
#include "GLWindow.h"


using namespace Eigen;

namespace CForge {
	GLWindow::GLWindow(void): CForgeObject("GLWindow") {
		m_pHandle = nullptr;
		m_pInputMan = nullptr;
	}//Constructor

	GLWindow::~GLWindow(void) {
		clear();
	}//Destructor

	void GLWindow::init(Vector2i Position, Vector2i Size, std::string Title, uint32_t GLMajorVersion, uint32_t GLMinorVersion) {
		clear();
		GLFWwindow* pWin = nullptr;

		if (GLMajorVersion == 0) GLMajorVersion = 4;
		if (GLMinorVersion == 0) GLMinorVersion = 6;

		if(GLMajorVersion >= 3) glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

		pWin = createGLWindow(Size.x(), Size.y(), Title, GLMajorVersion, GLMinorVersion);

		if (nullptr == pWin) {
			GLMajorVersion = 4;
			GLMinorVersion = 6;
			while (nullptr == pWin && GLMinorVersion > 1) {	
				pWin = createGLWindow(Size.x(), Size.y(), Title, GLMajorVersion, GLMinorVersion);
				GLMinorVersion -= 1;
			}	
		}

		if (nullptr == pWin) {
			GLMajorVersion = 3;
			GLMinorVersion = 3;
			pWin = createGLWindow(Size.x(), Size.y(), Title, GLMajorVersion, GLMinorVersion);
		}

		if (nullptr == pWin) {
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
			#ifdef __OPENGL_ES
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ES_API);
			#endif
			pWin = createGLWindow(Size.x(), Size.y(), Title, 1, 0);
		}		
		if (nullptr == pWin) throw CForgeExcept("Failed to crate OpenGL window. OpenGL seems not to be available!");

		glfwMakeContextCurrent(pWin);

		// initialize glad
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

#ifdef __OPENGLES__
		gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress);

		// glad does not load these on default with OpenGL es
		glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)glfwGetProcAddress("glGetUniformBlockIndex");
		glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)glfwGetProcAddress("glUniformBlockBinding");
		glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
		glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
		glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)glfwGetProcAddress("glDrawRangeElements");
		glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)glfwGetProcAddress("glBindBufferBase");

		glBeginQuery = (PFNGLBEGINQUERYPROC)glfwGetProcAddress("glBeginQuery");
		glEndQuery = (PFNGLENDQUERYPROC)glfwGetProcAddress("glEndQuery");
		glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)glfwGetProcAddress("glGetQueryObjectuiv");
#endif
		glViewport(0, 0, Size.x(), Size.y());
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		m_pHandle = pWin;

		// register keyboard and mouse
		m_pInputMan = SInputManager::instance();
		m_Mouse.init(pWin);
		m_Keyboard.init(pWin);
		m_Character.init(pWin);
		m_pInputMan->registerDevice(pWin, &m_Keyboard);
		m_pInputMan->registerDevice(pWin, &m_Mouse);
		m_pInputMan->registerDevice(pWin, &m_Character);

	}//initialize

	GLFWwindow* GLWindow::createGLWindow(uint32_t Width, uint32_t Height, std::string Title, uint32_t GLMajorVersion, uint32_t GLMinorVersion) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLMajorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLMinorVersion);
		GLFWwindow* pRval = glfwCreateWindow(Width, Height, Title.c_str(), nullptr, nullptr);
		m_Title = Title;
		return pRval;
	}//createGLWindow

	void GLWindow::clear(void) {
		if (m_pInputMan != nullptr) {
			m_pInputMan->unregisterDevice(&m_Mouse);
			m_pInputMan->unregisterDevice(&m_Keyboard);
			m_pInputMan->unregisterDevice(&m_Character);
			m_pInputMan->release();
		}
		m_pInputMan = nullptr;	
		m_pHandle = nullptr;
		m_Mouse.clear();
		m_Keyboard.clear();
		m_Character.clear();
	}//clear

	void GLWindow::update(void) {
		glfwPollEvents();
	}//update

	void GLWindow::swapBuffers(void) {
		glfwSwapBuffers((GLFWwindow*)m_pHandle);
	}//swapBuffers

	bool GLWindow::shutdown(void) {
		return (nullptr == m_pHandle || glfwWindowShouldClose((GLFWwindow*)m_pHandle));
	}//shutdown

	void* GLWindow::handle(void)const {
		return m_pHandle;
	}//handle

	void GLWindow::closeWindow(void) {
		if(nullptr != m_pHandle) glfwDestroyWindow((GLFWwindow*)m_pHandle);
		m_pHandle = nullptr;
	}//closeWindow

	uint32_t GLWindow::width(void)const {
		int32_t Rval = 0;
		glfwGetWindowSize((GLFWwindow*)m_pHandle, &Rval, nullptr);
		return (uint32_t)Rval;
	}//width

	uint32_t GLWindow::height(void)const {
		int32_t Rval = 0;
		glfwGetWindowSize((GLFWwindow*)m_pHandle, nullptr, &Rval);
		return Rval;
	}//height

	Keyboard* GLWindow::keyboard(void) {
		return &m_Keyboard;
	}//keyboard

	Mouse* GLWindow::mouse(void) {
		return &m_Mouse;
	}//mouse

	std::string GLWindow::title(void)const {
		return m_Title;
	}//title

	void GLWindow::title(const std::string Title) {
		m_Title = Title;
		glfwSetWindowTitle((GLFWwindow*)m_pHandle, m_Title.c_str());
	}//title

	Character* GLWindow::character(void) {
		return &m_Character;
	}//mouse
}//name space