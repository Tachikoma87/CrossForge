#include "TempRegAppState.h"

#include <GLFW/glfw3.h>

#include "../../CForge/Core/CrossForgeException.h"

namespace TempReg {
	TempRegAppState::TempRegAppState(void) :
		m_GuiCaptureMouse(false), m_GuiCaptureKeyboard(false), m_TakeScreenshot(false), m_glfwCursorLock(false), 
		m_FocussedViewport(-1), m_AppInteractionMode(AppInteractionMode::VIEWING) {
	
	}//Constructor

	TempRegAppState::~TempRegAppState() {

	}//Destructor

	// Setters

	void TempRegAppState::queueScreenshot(bool rVal) {
		m_TakeScreenshot = rVal;
	}//queueScreenshot

	void TempRegAppState::lockCursor(void* pHandle) {
		glfwSetInputMode((GLFWwindow*)pHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_glfwCursorLock = true;
	}//lockCursor

	void TempRegAppState::unlockCursor(void* pHandle) {
		glfwSetInputMode((GLFWwindow*)pHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_glfwCursorLock = false;
	}//unlockCursor

	void TempRegAppState::focusViewport(int32_t VPIndex) {
		m_FocussedViewport = VPIndex;
	}//focusDatasetView

	void TempRegAppState::interactionMode(AppInteractionMode Mode) {
		m_AppInteractionMode = Mode;
	}//interactionMode

	//Getters

	bool TempRegAppState::mouseCapturedByGui(void) const {
		return m_GuiCaptureMouse;
	}//mouseCapturedByGui

	bool TempRegAppState::keyboardCapturedByGui(void) const {
		return m_GuiCaptureKeyboard;
	}//keyboardCapturedByGui

	bool TempRegAppState::screenshotQueued(void) const {
		return m_TakeScreenshot;
	}//screenshotQueued

	bool TempRegAppState::cursorLocked(void) const {
		return m_glfwCursorLock;
	}//cursorLocked

	int32_t TempRegAppState::focussedViewport(void) const {
		return m_FocussedViewport;
	}//focussedDatasetView

	AppInteractionMode TempRegAppState::activeInteractionMode(void) const {
		return m_AppInteractionMode;
	}//activeInteractionMode
}