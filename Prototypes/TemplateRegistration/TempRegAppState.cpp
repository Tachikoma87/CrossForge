#include "TempRegAppState.h"

#include <GLFW/glfw3.h>

#include "../../CForge/Core/CrossForgeException.h"

namespace TempReg {
	TempRegAppState::TempRegAppState(void) :
		m_GuiCaptureMouse(false), m_GuiCaptureKeyboard(false), m_TakeScreenshot(false), m_glfwCursorLock(false),
		m_MouseBtnOldState{ false }, m_MouseBtnViewportFocus(-1) {

	}//Constructor

	TempRegAppState::~TempRegAppState() {

	}//Destructor

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

	void TempRegAppState::currentPickResClick(PickingResult PickRes) {
		m_PickResClick = PickRes;
	}//currentPickResClick

	void TempRegAppState::oldPickResClick(PickingResult PickRes) {
		m_OldPickResClick = PickRes;
	}//oldPickResClick

	void TempRegAppState::currentPickResHover(PickingResult PickRes) {
		m_PickResHover = PickRes;
	}//currentPickResHover

	void TempRegAppState::oldPickResHover(PickingResult PickRes) {
		m_OldPickResHover = PickRes;
	}//oldPickResHover

	void TempRegAppState::manualCorrespondenceTemplate(size_t VertexID, CorrespondenceType CT) {
		if (CT == CorrespondenceType::NONE) throw CForgeExcept("Invalid correspondence type");

		m_ManualCorrespondence.TemplatePointUID = VertexID;
		m_ManualCorrespondence.Type = CT;

		m_ManualCorrTemplateReady = true;
	}//manualCorrespondenceTemplate

	void TempRegAppState::manualCorrespondenceTarget(int64_t VertexID) {
		m_ManualCorrespondence.TargetPoint.UID = VertexID;
		m_ManualCorrespondence.TargetPoint.IsVertex = true;
		m_ManualCorrespondence.TargetPoint.Position = Vector3f::Zero();

		m_ManualCorrTargetReady = true;
	}//manualCorrespondenceTarget

	void TempRegAppState::manualCorrespondenceTarget(Vector3f Position) {
		m_ManualCorrespondence.TargetPoint.UID = 0;
		m_ManualCorrespondence.TargetPoint.IsVertex = false;
		m_ManualCorrespondence.TargetPoint.Position = Position;

		m_ManualCorrTargetReady = true;
	}//manualCorrespondenceTarget

	void TempRegAppState::clearManualCorrespondence(void) {
		m_ManualCorrespondence.TemplatePointUID = 0;
		m_ManualCorrespondence.TargetPoint.UID = 0;
		m_ManualCorrespondence.TargetPoint.IsVertex = false;
		m_ManualCorrespondence.TargetPoint.Position = Vector3f::Zero();
		m_ManualCorrespondence.Type = CorrespondenceType::NONE;

		m_ManualCorrTemplateReady = false;
		m_ManualCorrTargetReady = false;
	}//clearManualCorrespondence

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

	PickingResult TempRegAppState::currentPickResClick(void) const {
		return m_PickResClick;
	}//currentPickResClick

	PickingResult TempRegAppState::oldPickResClick(void) const {
		return m_OldPickResClick;
	}//oldPickResClick

	PickingResult TempRegAppState::currentPickResHover(void) const {
		return m_PickResHover;
	}//currentPickResHover

	PickingResult TempRegAppState::oldPickResHover(void) const {
		return m_OldPickResHover;
	}//oldPickResHover

	bool TempRegAppState::manualCorrespondenceReady(void) const {
		return m_ManualCorrTemplateReady && m_ManualCorrTargetReady;
	}//manualCorrespondenceReady

	CorrespondencePair TempRegAppState::manualCorrespondence(void) const {
		return m_ManualCorrespondence;
	}//manualCorrespondence
}