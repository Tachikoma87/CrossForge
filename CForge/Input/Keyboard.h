/*****************************************************************************\
*                                                                           *
* File(s): Keyboard.h and Keyboard.cpp                                      *
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
#ifndef __CFORGE_KEYBOARD_H__
#define __CFORGE_KEYBOARD_H__

#include "../Core/CForgeObject.h"

namespace CForge {
	class CFORGE_IXPORT Keyboard: public CForgeObject {
	public:
		enum Key : int16_t {
			KEY_UNKNOWN = -1,

			KEY_0 = 48,
			KEY_1 = 49,
			KEY_2 = 50,
			KEY_3 = 51,
			KEY_4 = 52,
			KEY_5 = 53,
			KEY_6 = 54,
			KEY_7 = 55,
			KEY_8 = 56,
			KEY_9 = 57,

			KEY_A = 97,
			KEY_B = 98,
			KEY_C = 99,
			KEY_D = 100,
			KEY_E = 101,
			KEY_F = 102,
			KEY_G = 103,
			KEY_H = 104,
			KEY_I = 105,
			KEY_J = 106,
			KEY_K = 107,
			KEY_L = 108,
			KEY_M = 109,
			KEY_N = 110,
			KEY_O = 111,
			KEY_P = 112,
			KEY_Q = 113,
			KEY_R = 114,
			KEY_S = 115,
			KEY_T = 116,
			KEY_U = 117,
			KEY_V = 118,
			KEY_W = 119,
			KEY_X = 120,
			KEY_Y = 121,
			KEY_Z = 122,
			
			KEY_F1,
			KEY_F2,
			KEY_F3,
			KEY_F4,
			KEY_F5,
			KEY_F6,
			KEY_F7,
			KEY_F8,
			KEY_F9,
			KEY_F10,
			KEY_F11,
			KEY_F12,

			KEY_SPACE,
			KEY_APOSTROPHE,
			KEY_COMMA,
			KEY_MINUS,
			KEY_PERIOD,
			KEY_SLASH,
			KEY_SEMICOLON,
			KEY_EQUAL,
			KEY_LEFT_BRACKET,
			KEY_BACKSLASH,
			KEY_RIGHT_BRACKET,
			KEY_GRAVE_ACCENT,
			KEY_ESCAPE,
			KEY_ENTER,
			KEY_TAB,
			KEY_BACKSPACE,
			KEY_INSERT,
			KEY_DELETE,
			KEY_RIGHT,
			KEY_LEFT,
			KEY_DOWN,
			KEY_UP,
			KEY_PAGE_UP,
			KEY_PAGE_DOWN,
			KEY_HOME,
			KEY_END,
			KEY_CAPS_LOCK,
			KEY_SCROLL_LOCK,
			KEY_NUM_LOCK,
			KEY_PRINT_SCREEN,
			KEY_PAUSE,

			// Keypad
			KEY_KP_0,
			KEY_KP_1,
			KEY_KP_2,
			KEY_KP_3,
			KEY_KP_4,
			KEY_KP_5,
			KEY_KP_6,
			KEY_KP_7,
			KEY_KP_8,
			KEY_KP_9,
			KEY_KP_DECIMAL,
			KEY_KP_DIVIDE,
			KEY_KP_MULTIPLY,
			KEY_KP_SUBTRACT,
			KEY_KP_ADD,
			KEY_KP_ENTER,
			KEY_KP_EQUAL,
			
			KEY_LEFT_SHIFT,
			KEY_LEFT_CONTROL,
			KEY_LEFT_ALT,
			KEY_RIGHT_SHIFT,
			KEY_RIGHT_CONTROL,
			KEY_RIGHT_ALT,

			KEY_MENU,

			KEY_COUNT,
		};

		enum State: int8_t {
			KEY_RELEASED = 0,
			KEY_PRESSED = 1,
			KEY_REPEATED = 2,
		};

		Keyboard(void);
		~Keyboard(void);

		void init(class GLFWwindow *pWin);
		void clear(void);

		bool keyPressed(Key K, bool Reset);
		bool keyPressed(Key K)const;
		void keyState(Key K, State S);
		State keyState(Key K)const;
	protected:

		State m_KeyStates[KEY_COUNT]; // 0 release, 1 pressed, 2 repeated
		class GLFWwindow* m_pWin;
	};//Keyboard

}//name-space

#endif