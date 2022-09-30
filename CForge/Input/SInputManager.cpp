#include <GLFW/glfw3.h>
#include "SInputManager.h"

namespace CForge {

	SInputManager* SInputManager::m_pInstance = nullptr;
	uint32_t SInputManager::m_InstanceCount = 0;

	SInputManager* SInputManager::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new SInputManager();
			m_pInstance->init();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void SInputManager::release(void) {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	SInputManager::SInputManager(void): CForgeObject("SInputManager") {

	}//Constructor

	SInputManager::~SInputManager(void) {
		clear();
	}//Destructor

	void SInputManager::init(void) {
		clear();

	}//initialize

	void SInputManager::clear(void) {

	}//clear

	void SInputManager::registerDevice(GLFWwindow* pWin, Keyboard* pKeyboard) {
		if (nullptr == pWin) throw NullpointerExcept("pWin");
		if (nullptr == pKeyboard) throw NullpointerExcept("pKeyboard");

		for (auto i : m_RegisteredKeyboards) {
			if (nullptr == i) continue;
			if (i->pWin == pWin && i->pKeyboard == pKeyboard) return; // we already know this pair
		}//for[all registered keyboards

		KeyboardEntity* pEntity = new KeyboardEntity();
		pEntity->pKeyboard = pKeyboard;
		pEntity->pWin = pWin;

		glfwSetKeyCallback(pWin, SInputManager::keyboardCallbackFunc);
		glfwSetCharCallback(pWin, SInputManager::characterCallbackFunc);

		for (auto& i : m_RegisteredKeyboards) {
			if (nullptr == i) {
				i = pEntity;
				pEntity = nullptr;
				break;
			}
		}//for[registered keyboards]

		if (nullptr != pEntity) m_RegisteredKeyboards.push_back(pEntity);

	}//registerDevice (keyboard)

	void SInputManager::registerDevice(GLFWwindow* pWin, Mouse* pMouse) {
		if (nullptr == pWin) throw NullpointerExcept("pWin");
		if (nullptr == pMouse) throw NullpointerExcept("pMouse");

		for (auto i : m_RegisteredMice) {
			if (nullptr == i) continue;
			if (i->pWin == pWin && i->pMouse == pMouse) return; // we already know this pair
		}//for[all registered mice]

		MouseEntity* pEntity = new MouseEntity();
		pEntity->pMouse = pMouse;
		pEntity->pWin = pWin;

		glfwSetMouseButtonCallback(pWin, SInputManager::mouseButtonCallbackFunc);
		glfwSetCursorPosCallback(pWin, SInputManager::mousePositionCallbackFunc);
		glfwSetScrollCallback(pWin, SInputManager::mouseWheelCallbackFunc);

		for (auto& i : m_RegisteredMice) {
			if (nullptr == i) {
				i = pEntity;
				pEntity = nullptr;
				break;
			}
		}//for[registered mice]

		if (nullptr != pEntity) m_RegisteredMice.push_back(pEntity);

	}//registerDevice (Mouse)

	void SInputManager::registerDevice(GLFWwindow* pWin, Character* pCharacter) {
		if (nullptr == pWin) throw NullpointerExcept("pWin");
		if (nullptr == pCharacter) throw NullpointerExcept("pCharacter");

		for (auto i : m_RegisteredCharacterCallbacks) {
			if (nullptr == i) continue;
			if (i->pWin == pWin && i->pCharacter == pCharacter) return; // we already know this pair
		}//for[all registered character callbacks]

		CharacterEntity* pEntity = new CharacterEntity();
		pEntity->pCharacter = pCharacter;
		pEntity->pWin = pWin;

		glfwSetCharCallback(pWin, SInputManager::characterCallbackFunc);

		for (auto& i : m_RegisteredCharacterCallbacks) {
			if (nullptr == i) {
				i = pEntity;
				pEntity = nullptr;
				break;
			}
		}//for[registered characters]

		if (nullptr != pEntity) m_RegisteredCharacterCallbacks.push_back(pEntity);

	}//registerDevice (Character)

	void SInputManager::unregisterDevice(Keyboard* pKeyboard) {
		for (auto& i : m_RegisteredKeyboards) {
			if (i != nullptr && i->pKeyboard == pKeyboard) {
				glfwSetKeyCallback(i->pWin, nullptr);
				delete i;
				i = nullptr;
			}
		}//for[all devices]
	}//unregisterKeyboard

	void SInputManager::unregisterDevice(Mouse* pMouse) {
		for (auto& i : m_RegisteredMice) {
			if (i != nullptr && i->pMouse == pMouse) {
				glfwSetMouseButtonCallback(i->pWin, nullptr);
				glfwSetCursorPosCallback(i->pWin, nullptr);
				glfwSetScrollCallback(i->pWin, nullptr);
				delete i;
				i = nullptr;
			}
		}//for[registered mice]
	}//unregister device (mouse)

	void SInputManager::unregisterDevice(Character* pCharacter) {
		for (auto& i : m_RegisteredCharacterCallbacks) {
			if (i != nullptr && i->pCharacter == pCharacter) {
				glfwSetCharCallback(i->pWin, nullptr);
				delete i;
				i = nullptr;
			}
		}//for[all character callbacks]
	}//unregisterCharacter


	void SInputManager::keyboardCallbackFunc(GLFWwindow* pWin, int Key, int Scancode, int Action, int Mods) {
		Keyboard::Key K = Keyboard::KEY_UNKNOWN;
		Keyboard::State S = Keyboard::KEY_RELEASED;

		switch (Key) {
		case GLFW_KEY_SPACE: K = Keyboard::KEY_SPACE; break;
		case GLFW_KEY_APOSTROPHE: K = Keyboard::KEY_APOSTROPHE; break;
		case GLFW_KEY_COMMA: K = Keyboard::KEY_COMMA; break;
		case GLFW_KEY_MINUS: K = Keyboard::KEY_MINUS; break;
		case GLFW_KEY_PERIOD: K = Keyboard::KEY_PERIOD; break;
		case GLFW_KEY_SLASH: K = Keyboard::KEY_SLASH; break;
		case GLFW_KEY_SEMICOLON: K = Keyboard::KEY_SEMICOLON; break;
		case GLFW_KEY_EQUAL: K = Keyboard::KEY_EQUAL; break;

		case GLFW_KEY_0: K = Keyboard::KEY_0; break;
		case GLFW_KEY_1: K = Keyboard::KEY_1; break;
		case GLFW_KEY_2: K = Keyboard::KEY_2; break;
		case GLFW_KEY_3: K = Keyboard::KEY_3; break;
		case GLFW_KEY_4: K = Keyboard::KEY_4; break;
		case GLFW_KEY_5: K = Keyboard::KEY_5; break;
		case GLFW_KEY_6: K = Keyboard::KEY_6; break;
		case GLFW_KEY_7: K = Keyboard::KEY_7; break;
		case GLFW_KEY_8: K = Keyboard::KEY_8; break;
		case GLFW_KEY_9: K = Keyboard::KEY_9; break;

		case GLFW_KEY_A: K = Keyboard::KEY_A; break;
		case GLFW_KEY_B: K = Keyboard::KEY_B; break;
		case GLFW_KEY_C: K = Keyboard::KEY_C; break;
		case GLFW_KEY_D: K = Keyboard::KEY_D; break;
		case GLFW_KEY_E: K = Keyboard::KEY_E; break;
		case GLFW_KEY_F: K = Keyboard::KEY_F; break;
		case GLFW_KEY_G: K = Keyboard::KEY_G; break;
		case GLFW_KEY_H: K = Keyboard::KEY_H; break;
		case GLFW_KEY_I: K = Keyboard::KEY_I; break;
		case GLFW_KEY_J: K = Keyboard::KEY_J; break;
		case GLFW_KEY_K: K = Keyboard::KEY_K; break;
		case GLFW_KEY_L: K = Keyboard::KEY_L; break;
		case GLFW_KEY_M: K = Keyboard::KEY_M; break;
		case GLFW_KEY_N: K = Keyboard::KEY_N; break;
		case GLFW_KEY_O: K = Keyboard::KEY_O; break;
		case GLFW_KEY_P: K = Keyboard::KEY_P; break;
		case GLFW_KEY_Q: K = Keyboard::KEY_Q; break;
		case GLFW_KEY_R: K = Keyboard::KEY_R; break;
		case GLFW_KEY_S: K = Keyboard::KEY_S; break;
		case GLFW_KEY_T: K = Keyboard::KEY_T; break;
		case GLFW_KEY_U: K = Keyboard::KEY_U; break;
		case GLFW_KEY_V: K = Keyboard::KEY_V; break;
		case GLFW_KEY_W: K = Keyboard::KEY_W; break;
		case GLFW_KEY_X: K = Keyboard::KEY_X; break;
		case GLFW_KEY_Y: K = Keyboard::KEY_Y; break;
		case GLFW_KEY_Z: K = Keyboard::KEY_Z; break;

		case GLFW_KEY_LEFT_BRACKET: K = Keyboard::KEY_LEFT_BRACKET; break;
		case GLFW_KEY_BACKSLASH: K = Keyboard::KEY_BACKSLASH; break;
		case GLFW_KEY_RIGHT_BRACKET: K = Keyboard::KEY_RIGHT_BRACKET; break;
		case GLFW_KEY_GRAVE_ACCENT: K = Keyboard::KEY_GRAVE_ACCENT; break;
		case GLFW_KEY_ESCAPE: K = Keyboard::KEY_ESCAPE; break;
		case GLFW_KEY_ENTER: K = Keyboard::KEY_ENTER; break;
		case GLFW_KEY_TAB: K = Keyboard::KEY_TAB; break;
		case GLFW_KEY_BACKSPACE: K = Keyboard::KEY_BACKSPACE; break;
		case GLFW_KEY_INSERT: K = Keyboard::KEY_INSERT; break;
		case GLFW_KEY_DELETE: K = Keyboard::KEY_DELETE; break;

		case GLFW_KEY_RIGHT: K = Keyboard::KEY_RIGHT; break;
		case GLFW_KEY_LEFT: K = Keyboard::KEY_LEFT; break;
		case GLFW_KEY_DOWN: K = Keyboard::KEY_DOWN; break;
		case GLFW_KEY_UP: K = Keyboard::KEY_UP; break;

		case GLFW_KEY_PAGE_UP: K = Keyboard::KEY_PAGE_UP; break;
		case GLFW_KEY_PAGE_DOWN: K = Keyboard::KEY_PAGE_DOWN; break;
		case GLFW_KEY_HOME: K = Keyboard::KEY_HOME; break;
		case GLFW_KEY_END: K = Keyboard::KEY_END; break;
		case GLFW_KEY_CAPS_LOCK: K = Keyboard::KEY_CAPS_LOCK; break;
		case GLFW_KEY_SCROLL_LOCK: K = Keyboard::KEY_SCROLL_LOCK; break;
		case GLFW_KEY_NUM_LOCK: K = Keyboard::KEY_NUM_LOCK; break;
		case GLFW_KEY_PRINT_SCREEN: K = Keyboard::KEY_PRINT_SCREEN; break;
		case GLFW_KEY_PAUSE: K = Keyboard::KEY_PAUSE; break;

		case GLFW_KEY_F1: K = Keyboard::KEY_F1; break;
		case GLFW_KEY_F2: K = Keyboard::KEY_F2; break;
		case GLFW_KEY_F3: K = Keyboard::KEY_F3; break;
		case GLFW_KEY_F4: K = Keyboard::KEY_F4; break;
		case GLFW_KEY_F5: K = Keyboard::KEY_F5; break;
		case GLFW_KEY_F6: K = Keyboard::KEY_F6; break;
		case GLFW_KEY_F7: K = Keyboard::KEY_F7; break;
		case GLFW_KEY_F8: K = Keyboard::KEY_F8; break;
		case GLFW_KEY_F9: K = Keyboard::KEY_F9; break;
		case GLFW_KEY_F10: K = Keyboard::KEY_F10; break;
		case GLFW_KEY_F11: K = Keyboard::KEY_F11; break;
		case GLFW_KEY_F12: K = Keyboard::KEY_F12; break;

		case GLFW_KEY_KP_0: K = Keyboard::KEY_KP_0; break;
		case GLFW_KEY_KP_1: K = Keyboard::KEY_KP_1; break;
		case GLFW_KEY_KP_2: K = Keyboard::KEY_KP_2; break;
		case GLFW_KEY_KP_3: K = Keyboard::KEY_KP_3; break;
		case GLFW_KEY_KP_4: K = Keyboard::KEY_KP_4; break;
		case GLFW_KEY_KP_5: K = Keyboard::KEY_KP_5; break;
		case GLFW_KEY_KP_6: K = Keyboard::KEY_KP_6; break;
		case GLFW_KEY_KP_7: K = Keyboard::KEY_KP_7; break;
		case GLFW_KEY_KP_8: K = Keyboard::KEY_KP_8; break;
		case GLFW_KEY_KP_9: K = Keyboard::KEY_KP_9; break;
		case GLFW_KEY_KP_DECIMAL: K = Keyboard::KEY_KP_DECIMAL; break;
		case GLFW_KEY_KP_DIVIDE: K = Keyboard::KEY_KP_DIVIDE; break;
		case GLFW_KEY_KP_MULTIPLY: K = Keyboard::KEY_KP_DIVIDE; break;
		case GLFW_KEY_KP_SUBTRACT: K = Keyboard::KEY_KP_SUBTRACT; break;
		case GLFW_KEY_KP_ADD: K = Keyboard::KEY_KP_ADD; break;
		case GLFW_KEY_KP_ENTER: K = Keyboard::KEY_KP_ENTER; break;
		case GLFW_KEY_KP_EQUAL: K = Keyboard::KEY_KP_EQUAL; break;

		case GLFW_KEY_LEFT_SHIFT: K = Keyboard::KEY_LEFT_SHIFT; break;
		case GLFW_KEY_LEFT_CONTROL: K = Keyboard::KEY_LEFT_CONTROL; break;
		case GLFW_KEY_LEFT_ALT: K = Keyboard::KEY_LEFT_ALT; break;
		case GLFW_KEY_RIGHT_SHIFT: K = Keyboard::KEY_RIGHT_SHIFT; break;
		case GLFW_KEY_RIGHT_CONTROL: K = Keyboard::KEY_RIGHT_CONTROL; break;
		case GLFW_KEY_RIGHT_ALT: K = Keyboard::KEY_RIGHT_ALT; break;
		case GLFW_KEY_MENU: K = Keyboard::KEY_MENU; break;

		default: break;
		}

		// release, pressed, or repeated?
		switch (Action) {
		case GLFW_PRESS: S = Keyboard::KEY_PRESSED; break;
		case GLFW_REPEAT: S = Keyboard::KEY_REPEATED; break;
		case GLFW_RELEASE: S = Keyboard::KEY_RELEASED; break;
		}

		if (K != Keyboard::KEY_UNKNOWN) {
			for (auto i : m_pInstance->m_RegisteredKeyboards) {
				if (nullptr != i && pWin == i->pWin) i->pKeyboard->keyState(K, S);
			}//for[registered keyboards]
		}

	}//keyboardCallbackFunc

	void SInputManager::characterCallbackFunc(class GLFWwindow* pWin, unsigned int Codepoint) {
		for (auto i : m_pInstance->m_RegisteredKeyboards) {
			if (nullptr != i && pWin == i->pWin) i->pKeyboard->textInput(Codepoint);
		}
	}//characterCallbackFunc

	void SInputManager::mousePositionCallbackFunc(GLFWwindow* pWin, double xPos, double yPos) {

		for (auto i : m_pInstance->m_RegisteredMice) {
			if (nullptr != i && pWin == i->pWin) i->pMouse->position(Eigen::Vector2f(xPos, yPos));
		}//for[registered mice]

	}//mousePositionCallbackFunc

	void SInputManager::mouseButtonCallbackFunc(GLFWwindow* pWin, int Button, int Action, int Mode) {
		Mouse::Button Btn = Mouse::BTN_UNKNOWN;
		bool State = false;

		switch (Button) {
		case GLFW_MOUSE_BUTTON_LEFT: Btn = Mouse::BTN_LEFT; break;
		case GLFW_MOUSE_BUTTON_RIGHT: Btn = Mouse::BTN_RIGHT; break;
		case GLFW_MOUSE_BUTTON_MIDDLE: Btn = Mouse::BTN_MIDDLE; break;
		case GLFW_MOUSE_BUTTON_4: Btn = Mouse::BTN_4; break;
		case GLFW_MOUSE_BUTTON_5: Btn = Mouse::BTN_5; break;
		case GLFW_MOUSE_BUTTON_6: Btn = Mouse::BTN_6; break;
		case GLFW_MOUSE_BUTTON_7: Btn = Mouse::BTN_7; break;
		case GLFW_MOUSE_BUTTON_8: Btn = Mouse::BTN_8; break;
		default: break;
		}

		State = (Action == GLFW_PRESS) ? true : false;

		if (Btn != Mouse::BTN_UNKNOWN) {
			for (auto i : m_pInstance->m_RegisteredMice) {
				if (i != nullptr && i->pWin == pWin) i->pMouse->buttonState(Btn, State);
			}
		}

	}//mouseButtonCallbackFunc

	void SInputManager::mouseWheelCallbackFunc(class GLFWwindow* pWin, double xOffset, double yOffset) {

		for (auto i : m_pInstance->m_RegisteredMice) {
			if (nullptr != i && pWin == i->pWin) i->pMouse->wheel(Eigen::Vector2f(xOffset, yOffset));
		}
	}//mouseWheelCallbackFunc

	//void SInputManager::characterCallbackFunc(class GLFWwindow* pWin, unsigned int codepoint) {
	//	for (auto i : m_pInstance->m_RegisteredCharacterCallbacks) {
	//		if (nullptr != i && pWin == i->pWin) i->pCharacter->sendChar(codepoint);
	//	}
	//}//characterCallbackFunc

}//name-space
