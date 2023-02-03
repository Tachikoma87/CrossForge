#ifdef __OPENGLES__
#define GLFW_INCLUDE_ES3
#endif

#include "OpenGLHeader.h"
#include <GLFW/glfw3.h>
#include "../Input/SInputManager.h"
#include "GLWindow.h"
#include "../Core/SLogger.h"
#include "../Utility/CForgeUtility.h"


using namespace Eigen;

namespace CForge {

	std::map<GLWindow*, GLFWwindow*> GLWindow::m_WindowList;

	void GLWindow::sizeCallback(GLFWwindow* pHandle, int Width, int Height) {
		
		// find corresponding window
		for (auto i : m_WindowList) {
			if (i.second == pHandle) {
				// create message
				GLWindowMsg Msg;
				Msg.pHandle = (void*)i.first;
				Msg.Code = GLWindowMsg::MC_RESIZE;
				Msg.iParam[0] = Width;
				Msg.iParam[1] = Height;
				i.first->broadcast(Msg);
			}
		}//for[available windows]

	}//sizeCallback


	GLWindow::GLWindow(void): CForgeObject("GLWindow") {
		m_pHandle = nullptr;
		m_pInputMan = nullptr;
	}//Constructor

	GLWindow::~GLWindow(void) {
		clear();
	}//Destructor

	void GLWindow::init(Vector2i Position, Vector2i Size, std::string WindowTitle, uint32_t Multisample, uint32_t GLMajorVersion, uint32_t GLMinorVersion) {
		clear();
		GLFWwindow* pWin = nullptr;


#if defined(__EMSCRIPTEN__)
		GLMajorVersion = 2;
		GLMinorVersion = 0;
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		pWin = createGLWindow(Size.x(), Size.y(), WindowTitle, GLMajorVersion, GLMinorVersion);
		if (nullptr == pWin) {
			printf("Failed creating OpenGL window!\n");
			throw CForgeExcept("Failed creating OpenGL window");
		}
		glfwMakeContextCurrent(pWin);

		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::string e = "GLEW init failed: ";
			throw CForgeExcept("Failed initialiing glew!");
		}

#else
		if (GLMajorVersion == 0) GLMajorVersion = 4;
		if (GLMinorVersion == 0) GLMinorVersion = 6;

		if(GLMajorVersion >= 3) glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

		if (Multisample > 0) {
			glfwWindowHint(GLFW_SAMPLES, Multisample);
		}

		pWin = createGLWindow(Size.x(), Size.y(), WindowTitle, GLMajorVersion, GLMinorVersion);

		if (nullptr == pWin) {
			GLMajorVersion = 4;
			GLMinorVersion = 6;
			while (nullptr == pWin && GLMinorVersion > 1) {	
				pWin = createGLWindow(Size.x(), Size.y(), WindowTitle, GLMajorVersion, GLMinorVersion);
				GLMinorVersion -= 1;
			}	
		}

		if (nullptr == pWin) {
			GLMajorVersion = 3;
			GLMinorVersion = 3;
			pWin = createGLWindow(Size.x(), Size.y(), WindowTitle, GLMajorVersion, GLMinorVersion);
		}

		if (nullptr == pWin) {
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
			#ifdef __OPENGL_ES
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ES_API);
			#endif
			pWin = createGLWindow(Size.x(), Size.y(), WindowTitle, 1, 0);
		}		
		if (nullptr == pWin) throw CForgeExcept("Failed to crate OpenGL window. OpenGL seems not to be available!");

		glfwMakeContextCurrent(pWin);
		// initialize glad
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		vsync(true);
#endif
		

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

		//if (Multisample > 0) glEnable(GL_MULTISAMPLE);

		m_pHandle = pWin;

		// register keyboard and mouse
		m_pInputMan = SInputManager::instance();
		m_Mouse.init(pWin);
		m_Keyboard.init(pWin);
		m_pInputMan->registerDevice(pWin, &m_Keyboard);
		m_pInputMan->registerDevice(pWin, &m_Mouse);

		glfwSetWindowSizeCallback((GLFWwindow*)this->m_pHandle, sizeCallback);
		m_WindowList.insert(std::pair<GLWindow*, GLFWwindow*>(this, pWin));

		std::string ErrorMsg;
		if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			SLogger::log("Not handled OpenGL error occurred before initialization of RenderDevice: " + ErrorMsg, "RenderDevice", SLogger::LOGTYPE_ERROR);
		}

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
			m_pInputMan->release();
		}
		m_pInputMan = nullptr;	
		m_pHandle = nullptr;
		m_Mouse.clear();
		m_Keyboard.clear();
	}//clear

	void GLWindow::update(void) {
		glfwPollEvents();
	}//update

	void GLWindow::swapBuffers(void) {
		glfwSwapBuffers((GLFWwindow*)m_pHandle);
	}//swapBuffers

	bool GLWindow::shutdown(void) const {
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

	void GLWindow::vsync(bool Enable, int8_t ThrottleFactor) {
		if (Enable) {
			glfwSwapInterval((ThrottleFactor >= 0) ? ThrottleFactor : 1);
			m_ThrottleFactor = ThrottleFactor;
			m_VSync = true;
		}
		else {
			glfwSwapInterval(0);
			m_VSync = false;
		}
	}//vsync

	bool GLWindow::vsync(int8_t *pThrottleFactor)const {
		if (nullptr != pThrottleFactor) (*pThrottleFactor) = m_ThrottleFactor;
		return m_VSync;
	}//vsync

	void GLWindow::makeCurrent(void)const {
		if(nullptr != m_pHandle)	glfwMakeContextCurrent((GLFWwindow*)m_pHandle);
	}//makeCurrent

}//name space