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
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SINPUTMANAGER_H__
#define __CFORGE_SINPUTMANAGER_H__

#include "../Core/CForgeObject.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Character.h"

namespace CForge {
	class CFORGE_API SInputManager: public CForgeObject {
	public:
		static SInputManager* instance(void);
		void release(void);

		void registerDevice(class GLFWwindow* pWin, Keyboard* pKeyboard);
		void registerDevice(class GLFWwindow* pWin, Mouse* pMouse);
		void registerDevice(class GLFWwindow* pWin, Character* pCharacter);
		void unregisterDevice(Keyboard* pKeyboard);
		void unregisterDevice(Mouse* pMouse);
		void unregisterDevice(Character* pCharacter);

	protected:
		SInputManager(void);
		~SInputManager(void);

		void init(void);
		void clear(void);
	private:
		static SInputManager* m_pInstance;
		static uint32_t m_InstanceCount;

		static void keyboardCallbackFunc(class GLFWwindow* pWin, int Key, int Scancode, int Action, int Mods);
		static void characterCallbackFunc(class GLFWwindow* pWin, unsigned int codepoint);
		static void mousePositionCallbackFunc(class GLFWwindow* pWin, double xPos, double yPos);
		static void mouseButtonCallbackFunc(class GLFWwindow* pWin, int Button, int Action, int Mode);
		static void mouseWheelCallbackFunc(class GLFWwindow* pWin, double xOffset, double yOffset);

		struct KeyboardEntity {
			Keyboard* pKeyboard;
			class GLFWwindow* pWin;
		};
		struct MouseEntity {
			Mouse* pMouse;
			class GLFWwindow* pWin;
		};
		struct CharacterEntity {
			Character* pCharacter;
			class GLFWwindow* pWin;
		};

		std::vector<KeyboardEntity*> m_RegisteredKeyboards;
		std::vector<MouseEntity*> m_RegisteredMice;
		std::vector<CharacterEntity*> m_RegisteredCharacterCallbacks;

	};//SInputManager

}//name-space


#endif 
