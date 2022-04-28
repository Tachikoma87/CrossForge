#ifndef __TEMPREG_TEMPREGAPPSTATE_H__
#define __TEMPREG_TEMPREGAPPSTATE_H__

#include <Eigen/Eigen>

#include <set>

namespace TempReg {
	
	enum AppInteractionMode : int8_t {
		VIEWING = 0,	// Mode: general dataset viewing
		CORR_PICK = 1,	// Mode: manual correspondence picking
		VERT_INSPECT	// Mode: inspecting vertex properties
	};

	class TempRegAppState {
	public:
		TempRegAppState(void);
		~TempRegAppState();

		// Setters
		void queueScreenshot(bool rVal);		
		void lockCursor(void* pHandle);
		void unlockCursor(void* pHandle);		
		void focusViewport(int32_t VPIndex);
		void interactionMode(AppInteractionMode Mode);

		// Getters
		bool screenshotQueued(void) const;
		bool mouseCapturedByGui(void) const;
		bool keyboardCapturedByGui(void) const;
		bool cursorLocked(void) const;
		int32_t focussedViewport(void) const;
		AppInteractionMode activeInteractionMode(void) const;
		
	private:
		bool m_GuiCaptureMouse, m_GuiCaptureKeyboard;
		bool m_TakeScreenshot;
		bool m_glfwCursorLock;	
		int32_t m_FocussedViewport;
		AppInteractionMode m_AppInteractionMode;
	};
}

#endif