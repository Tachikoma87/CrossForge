#include "TempRegAppState.h"

#include <GLFW/glfw3.h>

#include "../../CForge/Core/CrossForgeException.h"

namespace TempReg {
	TempRegAppState::TempRegAppState(void) :
		m_GuiCaptureMouse(false), m_GuiCaptureKeyboard(false), m_TakeScreenshot(false), m_glfwCursorLock(false),
		m_MouseBtnOldState{ false }, m_MouseBtnViewportFocus(-1), m_AppInteractionMode(AppInteractionMode::VIEWING) {
	
		m_PreviousHoverPickResult.DT = DatasetType::NONE;
		m_PreviousHoverPickResult.VertexID = 0;
		m_PreviousHoverPickResult.ViewportID = 0;

		m_CurrentHoverPickResult.DT = DatasetType::NONE;
		m_CurrentHoverPickResult.VertexID = 0;
		m_CurrentHoverPickResult.ViewportID = 0;

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

	void TempRegAppState::oldMouseButtonState(CForge::Mouse::Button Btn, bool Down) {
		if (Btn < CForge::Mouse::BTN_LEFT || Btn > CForge::Mouse::BTN_MIDDLE) throw IndexOutOfBoundsExcept("Btn");
		m_MouseBtnOldState[Btn] = Down;
	}//mouseButtonOldState

	void TempRegAppState::mouseButtonViewportFocus(int32_t VPIndex) {
		m_MouseBtnViewportFocus = VPIndex;
	}//mouseButtonViewportFocus

	void TempRegAppState::currentMMBCursorPos(Vector2f Pos) {
		m_CursorPosMMB = Pos;
	}//currentMMBCursorPos

	void TempRegAppState::oldMMBCursorPos(Vector2f Pos) {
		m_OldCursorPosMMB = Pos;
	}//oldMMBCursorPos

	void TempRegAppState::interactionMode(AppInteractionMode Mode) {
		m_AppInteractionMode = Mode;
	}//interactionMode

	void TempRegAppState::newHoverPickResult(DatasetType DT, uint32_t VertexID, size_t ViewportID) {
		m_PreviousHoverPickResult.DT = m_CurrentHoverPickResult.DT;
		m_PreviousHoverPickResult.VertexID = m_CurrentHoverPickResult.VertexID;
		m_PreviousHoverPickResult.ViewportID = m_CurrentHoverPickResult.ViewportID;

		m_CurrentHoverPickResult.DT = DT;
		m_CurrentHoverPickResult.VertexID = VertexID;
		m_CurrentHoverPickResult.ViewportID = ViewportID;
	}//newHoverPickResult

	void TempRegAppState::pickingSuccessful(bool Res) {
		m_PickingSuccessful = Res;
	}//pickingSuccessful

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

	bool TempRegAppState::oldMouseButtonState(CForge::Mouse::Button Btn) const {
		if (Btn <= CForge::Mouse::BTN_UNKNOWN || Btn > CForge::Mouse::BTN_MIDDLE) throw IndexOutOfBoundsExcept("Btn");
		return m_MouseBtnOldState[Btn];
	}//mouseButtonOldState

	int32_t TempRegAppState::mouseButtonViewportFocus(void) const {
		return m_MouseBtnViewportFocus;
	}//mouseButtonViewportFocus

	Vector2f TempRegAppState::currentMMBCursorPos(void) const {
		return m_CursorPosMMB;
	}//currentMMBCursorPos

	Vector2f TempRegAppState::oldMMBCursorPos(void) const {
		return m_OldCursorPosMMB;
	}//oldMMBCursorPos

	AppInteractionMode TempRegAppState::activeInteractionMode(void) const {
		return m_AppInteractionMode;
	}//activeInteractionMode

	VertexPickResult TempRegAppState::currentHoverPickResult(void) const {
		return m_CurrentHoverPickResult;
	}//currentHoverPickResult

	VertexPickResult TempRegAppState::previousHoverPickResult(void) const {
		return m_PreviousHoverPickResult;
	}//lastHoverPickResult

	bool TempRegAppState::pickingSuccessful(void) const {
		return m_PickingSuccessful;
	}//pickingSuccessful
}