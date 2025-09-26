#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <crossforge/Utility/CForgeUtility.h>
#include "WindowSystem.h"

namespace CForge {

	GLFWwindow* createGLWindow(uint32_t Width, uint32_t Height, std::string Title, uint32_t GLMajorVersion, uint32_t GLMinorVersion) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLMajorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLMinorVersion);
		GLFWwindow* pRval = glfwCreateWindow(Width, Height, Title.c_str(), nullptr, nullptr);
		return pRval;
	}//createGLWindow



	WindowSystem::WindowSystem(): SystemBase(WindowSystem::identification) {

	}

	WindowSystem::~WindowSystem() {

	}

	void WindowSystem::initialize() {
		clear();
	}

	void WindowSystem::clear() {
		m_entities.clear();
	}

	void WindowSystem::update() {
		glfwPollEvents();

		for (EntityBasePtr pEntity : m_entities) {
			WindowEntityPtr pWinEntity = std::static_pointer_cast<WindowEntity>(pEntity);

			auto pGlfwWin = (GLFWwindow*)pWinEntity->getWindowPropertiesComponent()->getGlfwWindow();
			if (nullptr == pGlfwWin) continue;

			if (glfwWindowShouldClose(pGlfwWin)) {
				LogInfo("Closing windows at entity " + std::to_string(pWinEntity->getEntityId()), "");
				glfwDestroyWindow(pGlfwWin);
				pWinEntity->getWindowPropertiesComponent()->setGlfwWindow(nullptr);
			}
		}
	}

	void WindowSystem::swapBuffers() {

		for (EntityBasePtr pEntity : m_entities) {
			WindowEntityPtr pWinEntity = std::static_pointer_cast<WindowEntity>(pEntity);
			auto pGlfwWin = (GLFWwindow*)pWinEntity->getWindowPropertiesComponent()->getGlfwWindow();
			if (nullptr == pGlfwWin) continue;

			glfwSwapBuffers(pGlfwWin);
		}
	}

	bool WindowSystem::isEntityValid(EntityBasePtr pEntity) const {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");

		bool result = true;
		if (0 != pEntity->getIdentification().compare(WindowEntity::identification)) result = false;
		if (!pEntity->hasComponent(WindowPropertiesComponent::identification)) result = false;

		return result;
	}

	bool WindowSystem::initOpenGLWindow(WindowEntityPtr pWinEntity) {
		if (nullptr == pWinEntity) throw NullpointerExcept("pWinEntity");
		WindowPropertiesComponentPtr pWinCfgComp = pWinEntity->getWindowPropertiesComponent();
		if (nullptr == pWinCfgComp) {
			LogError("Provided windows entity has no window config component. Can not initialize OpenGL window!", "");
			return false;
		}

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


		if (pWinCfgComp->glMajorVersion() == 0) pWinCfgComp->glMajorVersion() = 4;
		if (pWinCfgComp->glMinorVersion() == 0) pWinCfgComp->glMinorVersion() = 6;

		if (pWinCfgComp->glMajorVersion() >= 3) glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

		if (pWinCfgComp->multisampling() > 0) {
			glfwWindowHint(GLFW_SAMPLES, pWinCfgComp->multisampling());
		}

		pWin = createGLWindow(pWinCfgComp->width(), pWinCfgComp->height(), pWinCfgComp->title(), pWinCfgComp->glMajorVersion(), pWinCfgComp->glMinorVersion());

		// if windows generation failed, try with other config
		if (nullptr == pWin) {
			uint32_t GLMajorVersion = 4;
			uint32_t GLMinorVersion = 6;
			while (nullptr == pWin && GLMinorVersion > 1) {
				pWin = createGLWindow(pWinCfgComp->width(),pWinCfgComp->height(), pWinCfgComp->title(), GLMajorVersion, GLMinorVersion);
				GLMinorVersion -= 1;
			}
		}

		if (nullptr == pWin) {
			uint32_t GLMajorVersion = 3;
			uint32_t GLMinorVersion = 3;
			pWin = createGLWindow(pWinCfgComp->width(), pWinCfgComp->height(), pWinCfgComp->title(), GLMajorVersion, GLMinorVersion);
		}

		if (nullptr == pWin) {
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#ifdef __OPENGL_ES
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ES_API);
#endif
			pWin = createGLWindow(pWinCfgComp->width(), pWinCfgComp->height(), pWinCfgComp->title(), 1, 0);
		}
		if (nullptr == pWin) throw CForgeExcept("Failed to crate OpenGL window. OpenGL seems not to be available!");

		glfwMakeContextCurrent(pWin);
		// initialize glad
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		changeVsync(pWinEntity, 1);
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

		glViewport(0, 0, pWinCfgComp->width(), pWinCfgComp->height());
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//if (Multisample > 0) glEnable(GL_MULTISAMPLE);
		pWinCfgComp->setGlfwWindow(pWin);

		//glfwSetWindowSizeCallback((GLFWwindow*)this->m_pHandle, sizeCallback);


		/*std::string ErrorMsg;
		if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			SLogger::log("Not handled OpenGL error occurred before initialization of RenderDevice: " + ErrorMsg, "RenderDevice", SLogger::LOGTYPE_ERROR);
		}*/

		return true;

	}

	void WindowSystem::changeVsync(WindowEntityPtr pEntity, int8_t vsyncFactor) {
		/*if (Enable) {
			glfwSwapInterval((ThrottleFactor >= 0) ? ThrottleFactor : 1);
			m_ThrottleFactor = ThrottleFactor;
			m_VSync = true;
		}
		else {
			glfwSwapInterval(0);
			m_VSync = false;
		}*/


	}
	


}