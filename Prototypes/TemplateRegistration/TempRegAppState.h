#ifndef __TEMPREG_TEMPREGAPPSTATE_H__
#define __TEMPREG_TEMPREGAPPSTATE_H__

#include "../../CForge/Input/Mouse.h"

#include <Eigen/Eigen>

#include <set>

#include "Dataset.h"

using namespace Eigen;

namespace TempReg {
	
	enum AppInteractionMode : int8_t {
		VIEWING = 0,	// Mode: general dataset viewing
		CORR_PICK = 1,	// Mode: manual correspondence picking
		VERT_INSPECT	// Mode: inspecting vertex properties
	};

	struct VertexPickResult {
		DatasetType DT;
		uint32_t VertexID;
		size_t ViewportID;
	};

	class TempRegAppState {
	public:
		TempRegAppState(void);
		~TempRegAppState();

		// Setters
		void queueScreenshot(bool rVal);		
		void lockCursor(void* pHandle);
		void unlockCursor(void* pHandle);
		void oldMouseButtonState(CForge::Mouse::Button Btn, bool Down);
		void mouseButtonViewportFocus(int32_t VPIndex);
		void currentMMBCursorPos(Vector2f Pos);
		void oldMMBCursorPos(Vector2f Pos);
		void interactionMode(AppInteractionMode Mode);
		void newHoverPickResult(DatasetType DT, uint32_t VertexID, size_t ViewportID);
		void pickingSuccessful(bool Res);

		// Getters
		bool screenshotQueued(void) const;
		bool mouseCapturedByGui(void) const;
		bool keyboardCapturedByGui(void) const;
		bool cursorLocked(void) const;
		bool oldMouseButtonState(CForge::Mouse::Button Btn) const;
		int32_t mouseButtonViewportFocus(void) const;
		Vector2f currentMMBCursorPos(void) const;
		Vector2f oldMMBCursorPos(void) const;
		AppInteractionMode activeInteractionMode(void) const;
		VertexPickResult currentHoverPickResult(void) const;
		VertexPickResult previousHoverPickResult(void) const;
		bool pickingSuccessful(void) const;
		
	private:
		bool m_GuiCaptureMouse, m_GuiCaptureKeyboard;
		bool m_TakeScreenshot;
		bool m_glfwCursorLock;
		bool m_MouseBtnOldState[3]; // LMB == [0], RMB == [1], MMB == [2]
		Vector2f m_CursorPosMMB, m_OldCursorPosMMB;
		int32_t m_MouseBtnViewportFocus;
		AppInteractionMode m_AppInteractionMode;
		bool m_PickingSuccessful;
		VertexPickResult m_CurrentHoverPickResult, m_PreviousHoverPickResult;
	};
}

#endif