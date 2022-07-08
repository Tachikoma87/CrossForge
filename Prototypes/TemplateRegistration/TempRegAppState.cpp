#include "TempRegAppState.h"

#include <GLFW/glfw3.h>

#include "../../CForge/Core/CrossForgeException.h"

namespace TempReg {
	TempRegAppState::TempRegAppState(void) :
		m_VPInteractionMode(ViewportInteractionMode::VIEWING), m_GuiCaptureMouse(false), m_GuiCaptureKeyboard(false), m_TakeScreenshot(false), m_glfwCursorLock(false),
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

	void TempRegAppState::prevMouseButtonState(CForge::Mouse::Button Btn, bool Down) {
		if (Btn < CForge::Mouse::BTN_LEFT || Btn > CForge::Mouse::BTN_MIDDLE) throw IndexOutOfBoundsExcept("Btn");
		m_MouseBtnOldState[Btn] = Down;
	}//prevMouseButtonState

	void TempRegAppState::viewportInteractionMode(ViewportInteractionMode Mode) {
		m_VPInteractionMode = Mode;
	}//viewportInteractionMode

	void TempRegAppState::mouseButtonViewportFocus(int32_t VPIndex) {
		m_MouseBtnViewportFocus = VPIndex;
	}//mouseButtonViewportFocus

	void TempRegAppState::currMMBCursorPos(Vector2f Pos) {
		m_CursorPosMMB = Pos;
	}//currMMBCursorPos

	void TempRegAppState::prevMMBCursorPos(Vector2f Pos) {
		m_OldCursorPosMMB = Pos;
	}//prevMMBCursorPos

	void TempRegAppState::rayIntersectionResult(const RayIntersectionResult Res) {
		m_RayIntersectRes = Res;
	}//rayIntersectionResult

	//void TempRegAppState::rayIntersectionResult(DatasetType DT, DatasetGeometryType GT, int IntersectedFace, int IntersectedVertex, Vector3f IntersectionPos, Vector3f BarycentricCoords) {
	//	m_RayIntersectRes.Dataset = DT;
	//	m_RayIntersectRes.GeometryType = GT;
	//	m_RayIntersectRes.Face = IntersectedFace;
	//	m_RayIntersectRes.Vertex = IntersectedVertex;
	//	m_RayIntersectRes.IntersectionPos = IntersectionPos;
	//	m_RayIntersectRes.BarycentricCoords = BarycentricCoords;
	//}//rayIntersectionResult

	void TempRegAppState::clearRayIntersectionResult(void) {
		m_RayIntersectRes.Dataset = DatasetType::NONE;
		m_RayIntersectRes.GeometryType = DatasetGeometryType::MESH;
		m_RayIntersectRes.Face = -1;
		m_RayIntersectRes.Vertex = -1;
		m_RayIntersectRes.IntersectionPos = Vector3f::Zero();
		m_RayIntersectRes.BarycentricCoords = Vector3f::Zero();
	}//clearRayIntersectionResult

	void TempRegAppState::prevSelectRes(PickingResult Res) {
		m_OldSelectRes = Res;
	}//prevSelectRes

	void TempRegAppState::prevHoverRes(PickingResult Res) {
		m_OldHoverRes = Res;
	}//prevHoverRes

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

	bool TempRegAppState::prevMouseButtonState(CForge::Mouse::Button Btn) const {
		if (Btn <= CForge::Mouse::BTN_UNKNOWN || Btn > CForge::Mouse::BTN_MIDDLE) throw IndexOutOfBoundsExcept("Btn");
		return m_MouseBtnOldState[Btn];
	}//prevMouseButtonState

	ViewportInteractionMode TempRegAppState::viewportInteractionMode(void) const {
		return m_VPInteractionMode;
	}//viewportInteractionMode

	int32_t TempRegAppState::mouseButtonViewportFocus(void) const {
		return m_MouseBtnViewportFocus;
	}//mouseButtonViewportFocus

	Vector2f TempRegAppState::currMMBCursorPos(void) const {
		return m_CursorPosMMB;
	}//currMMBCursorPos

	Vector2f TempRegAppState::prevMMBCursorPos(void) const {
		return m_OldCursorPosMMB;
	}//prevMMBCursorPos

	const TempRegAppState::RayIntersectionResult& TempRegAppState::rayIntersectionResult(void) const {
		return m_RayIntersectRes;
	}//rayIntersectionResult

	const TempRegAppState::PickingResult& TempRegAppState::prevSelectRes(void) const {
		return m_OldSelectRes;
	}//prevSelectRes

	const TempRegAppState::PickingResult& TempRegAppState::prevHoverRes(void) const {
		return m_OldHoverRes;
	}//prevHoverRes
}