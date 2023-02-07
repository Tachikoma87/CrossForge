/*****************************************************************************\
*                                                                           *
* File(s): SInputManager.h and SInputManager.cpp                           *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SINPUTMANAGER_H__
#define __CFORGE_SINPUTMANAGER_H__

#include "../Core/CForgeObject.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace CForge {
	class CFORGE_API SInputManager: public CForgeObject {
	public:
		static SInputManager* instance(void);
		void release(void);

		void registerDevice(struct GLFWwindow* pWin, Keyboard* pKeyboard);
		void registerDevice(struct GLFWwindow* pWin, Mouse* pMouse);
		void unregisterDevice(Keyboard* pKeyboard);
		void unregisterDevice(Mouse* pMouse);

	protected:
		SInputManager(void);
		~SInputManager(void);

		void init(void);
		void clear(void);
	private:
		static SInputManager* m_pInstance;
		static uint32_t m_InstanceCount;

		static void keyboardCallbackFunc(struct GLFWwindow* pWin, int Key, int Scancode, int Action, int Mods);
		static void characterCallbackFunc(struct GLFWwindow* pWin, unsigned int codepoint);
		static void mousePositionCallbackFunc(struct GLFWwindow* pWin, double xPos, double yPos);
		static void mouseButtonCallbackFunc(struct GLFWwindow* pWin, int Button, int Action, int Mode);
		static void mouseWheelCallbackFunc(struct GLFWwindow* pWin, double xOffset, double yOffset);

		struct KeyboardEntity {
			Keyboard* pKeyboard;
			struct GLFWwindow* pWin;
		};
		struct MouseEntity {
			Mouse* pMouse;
			struct GLFWwindow* pWin;
		};
		

		std::vector<KeyboardEntity*> m_RegisteredKeyboards;
		std::vector<MouseEntity*> m_RegisteredMice;

	};//SInputManager

}//name-space


#endif 
