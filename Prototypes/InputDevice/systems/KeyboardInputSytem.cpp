#include <GLFW/glfw3.h>
#include <crossforge/ecs/SEntityManager.h>
#include <crossforge/graphics/entities/WindowEntity.h>
#include "KeyboardInputSystem.h"
#include <crossforge/ecs/SSystemManager.h>

namespace crossforge {

	KeyboardInputSystem::KeyboardInputSystem(): SystemBase(KeyboardInputSystem::identification) {
		m_inheritance.push_back(KeyboardInputSystem::identification);
	}

	KeyboardInputSystem::~KeyboardInputSystem() {

	}


	void KeyboardInputSystem::initialize() {

	}
	void KeyboardInputSystem::clear() {

	}
	void KeyboardInputSystem::update() {
		// all keyboard input functionality is realized by the glfw callbacks, thus update method has nothing to do
	}

	bool KeyboardInputSystem::registerEntity(EntityBasePtr pEntity) {
		bool result = SystemBase::registerEntity(pEntity);
		if (result) {
			auto pInputEntity = EntityBase::cast<InputDeviceEntity>(pEntity);

			uint64_t windowEntityId = pInputEntity->getAssociatedWindowComponent()->windowEntityId();
			auto pWinEntity = EntityManager::getInstance()->getEntity<WindowEntity>(windowEntityId);
			auto pGlfwWindowHandle = (GLFWwindow*)pWinEntity->getWindowPropertiesComponent()->getGlfwWindowHandle();
			if (nullptr == pGlfwWindowHandle) {
				LogError("Window handle is nullptr. can not register keybaord input device entity.");
			}
			else {
				if (!pInputEntity->hasComponent(KeyboardDataComponent::identification)) pInputEntity->addComponent(std::make_shared<KeyboardDataComponent>());
				glfwSetKeyCallback(pGlfwWindowHandle, KeyboardInputSystem::keyboardCallbackFunc);
				if (m_handleToEntityMap.end() == m_handleToEntityMap.find(pGlfwWindowHandle)) m_handleToEntityMap.insert(std::pair((void*)pGlfwWindowHandle, std::vector<InputDeviceEntityPtr>()));
				m_handleToEntityMap.find(pGlfwWindowHandle)->second.push_back(pInputEntity);
				result = true;
			}

		}
		return result;
	}

	bool KeyboardInputSystem::isEntityValid(EntityBasePtr pEntity)const {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		bool result = true;
		if (!pEntity->isInstanceOf(InputDeviceEntity::identificaiton)) result = false;
		if (!pEntity->hasComponent(AssociatedWindowComponent::identification)) result = false;
		return result;
	}

	void KeyboardInputSystem::keyboardCallbackFunc(struct GLFWwindow* pWin, int Key, int Scancode, int Action, int Mods) {
		KeyboardDataComponent::Key K = KeyboardDataComponent::KEY_UNKNOWN;
		KeyboardDataComponent::KeyState S = KeyboardDataComponent::KEYSTATE_RELEASED;

		switch (Key) {
		case GLFW_KEY_SPACE: K = KeyboardDataComponent::KEY_SPACE; break;
		case GLFW_KEY_APOSTROPHE: K = KeyboardDataComponent::KEY_APOSTROPHE; break;
		case GLFW_KEY_COMMA: K = KeyboardDataComponent::KEY_COMMA; break;
		case GLFW_KEY_MINUS: K = KeyboardDataComponent::KEY_MINUS; break;
		case GLFW_KEY_PERIOD: K = KeyboardDataComponent::KEY_PERIOD; break;
		case GLFW_KEY_SLASH: K = KeyboardDataComponent::KEY_SLASH; break;
		case GLFW_KEY_SEMICOLON: K = KeyboardDataComponent::KEY_SEMICOLON; break;
		case GLFW_KEY_EQUAL: K = KeyboardDataComponent::KEY_EQUAL; break;

		case GLFW_KEY_0: K = KeyboardDataComponent::KEY_0; break;
		case GLFW_KEY_1: K = KeyboardDataComponent::KEY_1; break;
		case GLFW_KEY_2: K = KeyboardDataComponent::KEY_2; break;
		case GLFW_KEY_3: K = KeyboardDataComponent::KEY_3; break;
		case GLFW_KEY_4: K = KeyboardDataComponent::KEY_4; break;
		case GLFW_KEY_5: K = KeyboardDataComponent::KEY_5; break;
		case GLFW_KEY_6: K = KeyboardDataComponent::KEY_6; break;
		case GLFW_KEY_7: K = KeyboardDataComponent::KEY_7; break;
		case GLFW_KEY_8: K = KeyboardDataComponent::KEY_8; break;
		case GLFW_KEY_9: K = KeyboardDataComponent::KEY_9; break;

		case GLFW_KEY_A: K = KeyboardDataComponent::KEY_A; break;
		case GLFW_KEY_B: K = KeyboardDataComponent::KEY_B; break;
		case GLFW_KEY_C: K = KeyboardDataComponent::KEY_C; break;
		case GLFW_KEY_D: K = KeyboardDataComponent::KEY_D; break;
		case GLFW_KEY_E: K = KeyboardDataComponent::KEY_E; break;
		case GLFW_KEY_F: K = KeyboardDataComponent::KEY_F; break;
		case GLFW_KEY_G: K = KeyboardDataComponent::KEY_G; break;
		case GLFW_KEY_H: K = KeyboardDataComponent::KEY_H; break;
		case GLFW_KEY_I: K = KeyboardDataComponent::KEY_I; break;
		case GLFW_KEY_J: K = KeyboardDataComponent::KEY_J; break;
		case GLFW_KEY_K: K = KeyboardDataComponent::KEY_K; break;
		case GLFW_KEY_L: K = KeyboardDataComponent::KEY_L; break;
		case GLFW_KEY_M: K = KeyboardDataComponent::KEY_M; break;
		case GLFW_KEY_N: K = KeyboardDataComponent::KEY_N; break;
		case GLFW_KEY_O: K = KeyboardDataComponent::KEY_O; break;
		case GLFW_KEY_P: K = KeyboardDataComponent::KEY_P; break;
		case GLFW_KEY_Q: K = KeyboardDataComponent::KEY_Q; break;
		case GLFW_KEY_R: K = KeyboardDataComponent::KEY_R; break;
		case GLFW_KEY_S: K = KeyboardDataComponent::KEY_S; break;
		case GLFW_KEY_T: K = KeyboardDataComponent::KEY_T; break;
		case GLFW_KEY_U: K = KeyboardDataComponent::KEY_U; break;
		case GLFW_KEY_V: K = KeyboardDataComponent::KEY_V; break;
		case GLFW_KEY_W: K = KeyboardDataComponent::KEY_W; break;
		case GLFW_KEY_X: K = KeyboardDataComponent::KEY_X; break;
		case GLFW_KEY_Y: K = KeyboardDataComponent::KEY_Y; break;
		case GLFW_KEY_Z: K = KeyboardDataComponent::KEY_Z; break;

		case GLFW_KEY_LEFT_BRACKET: K = KeyboardDataComponent::KEY_LEFT_BRACKET; break;
		case GLFW_KEY_BACKSLASH: K = KeyboardDataComponent::KEY_BACKSLASH; break;
		case GLFW_KEY_RIGHT_BRACKET: K = KeyboardDataComponent::KEY_RIGHT_BRACKET; break;
		case GLFW_KEY_GRAVE_ACCENT: K = KeyboardDataComponent::KEY_GRAVE_ACCENT; break;
		case GLFW_KEY_ESCAPE: K = KeyboardDataComponent::KEY_ESCAPE; break;
		case GLFW_KEY_ENTER: K = KeyboardDataComponent::KEY_ENTER; break;
		case GLFW_KEY_TAB: K = KeyboardDataComponent::KEY_TAB; break;
		case GLFW_KEY_BACKSPACE: K = KeyboardDataComponent::KEY_BACKSPACE; break;
		case GLFW_KEY_INSERT: K = KeyboardDataComponent::KEY_INSERT; break;
		case GLFW_KEY_DELETE: K = KeyboardDataComponent::KEY_DELETE; break;

		case GLFW_KEY_RIGHT: K = KeyboardDataComponent::KEY_RIGHT; break;
		case GLFW_KEY_LEFT: K = KeyboardDataComponent::KEY_LEFT; break;
		case GLFW_KEY_DOWN: K = KeyboardDataComponent::KEY_DOWN; break;
		case GLFW_KEY_UP: K = KeyboardDataComponent::KEY_UP; break;

		case GLFW_KEY_PAGE_UP: K = KeyboardDataComponent::KEY_PAGE_UP; break;
		case GLFW_KEY_PAGE_DOWN: K = KeyboardDataComponent::KEY_PAGE_DOWN; break;
		case GLFW_KEY_HOME: K = KeyboardDataComponent::KEY_HOME; break;
		case GLFW_KEY_END: K = KeyboardDataComponent::KEY_END; break;
		case GLFW_KEY_CAPS_LOCK: K = KeyboardDataComponent::KEY_CAPS_LOCK; break;
		case GLFW_KEY_SCROLL_LOCK: K = KeyboardDataComponent::KEY_SCROLL_LOCK; break;
		case GLFW_KEY_NUM_LOCK: K = KeyboardDataComponent::KEY_NUM_LOCK; break;
		case GLFW_KEY_PRINT_SCREEN: K = KeyboardDataComponent::KEY_PRINT_SCREEN; break;
		case GLFW_KEY_PAUSE: K = KeyboardDataComponent::KEY_PAUSE; break;

		case GLFW_KEY_F1: K = KeyboardDataComponent::KEY_F1; break;
		case GLFW_KEY_F2: K = KeyboardDataComponent::KEY_F2; break;
		case GLFW_KEY_F3: K = KeyboardDataComponent::KEY_F3; break;
		case GLFW_KEY_F4: K = KeyboardDataComponent::KEY_F4; break;
		case GLFW_KEY_F5: K = KeyboardDataComponent::KEY_F5; break;
		case GLFW_KEY_F6: K = KeyboardDataComponent::KEY_F6; break;
		case GLFW_KEY_F7: K = KeyboardDataComponent::KEY_F7; break;
		case GLFW_KEY_F8: K = KeyboardDataComponent::KEY_F8; break;
		case GLFW_KEY_F9: K = KeyboardDataComponent::KEY_F9; break;
		case GLFW_KEY_F10: K = KeyboardDataComponent::KEY_F10; break;
		case GLFW_KEY_F11: K = KeyboardDataComponent::KEY_F11; break;
		case GLFW_KEY_F12: K = KeyboardDataComponent::KEY_F12; break;

		case GLFW_KEY_KP_0: K = KeyboardDataComponent::KEY_KP_0; break;
		case GLFW_KEY_KP_1: K = KeyboardDataComponent::KEY_KP_1; break;
		case GLFW_KEY_KP_2: K = KeyboardDataComponent::KEY_KP_2; break;
		case GLFW_KEY_KP_3: K = KeyboardDataComponent::KEY_KP_3; break;
		case GLFW_KEY_KP_4: K = KeyboardDataComponent::KEY_KP_4; break;
		case GLFW_KEY_KP_5: K = KeyboardDataComponent::KEY_KP_5; break;
		case GLFW_KEY_KP_6: K = KeyboardDataComponent::KEY_KP_6; break;
		case GLFW_KEY_KP_7: K = KeyboardDataComponent::KEY_KP_7; break;
		case GLFW_KEY_KP_8: K = KeyboardDataComponent::KEY_KP_8; break;
		case GLFW_KEY_KP_9: K = KeyboardDataComponent::KEY_KP_9; break;
		case GLFW_KEY_KP_DECIMAL: K = KeyboardDataComponent::KEY_KP_DECIMAL; break;
		case GLFW_KEY_KP_DIVIDE: K = KeyboardDataComponent::KEY_KP_DIVIDE; break;
		case GLFW_KEY_KP_MULTIPLY: K = KeyboardDataComponent::KEY_KP_DIVIDE; break;
		case GLFW_KEY_KP_SUBTRACT: K = KeyboardDataComponent::KEY_KP_SUBTRACT; break;
		case GLFW_KEY_KP_ADD: K = KeyboardDataComponent::KEY_KP_ADD; break;
		case GLFW_KEY_KP_ENTER: K = KeyboardDataComponent::KEY_KP_ENTER; break;
		case GLFW_KEY_KP_EQUAL: K = KeyboardDataComponent::KEY_KP_EQUAL; break;

		case GLFW_KEY_LEFT_SHIFT: K = KeyboardDataComponent::KEY_LEFT_SHIFT; break;
		case GLFW_KEY_LEFT_CONTROL: K = KeyboardDataComponent::KEY_LEFT_CONTROL; break;
		case GLFW_KEY_LEFT_ALT: K = KeyboardDataComponent::KEY_LEFT_ALT; break;
		case GLFW_KEY_RIGHT_SHIFT: K = KeyboardDataComponent::KEY_RIGHT_SHIFT; break;
		case GLFW_KEY_RIGHT_CONTROL: K = KeyboardDataComponent::KEY_RIGHT_CONTROL; break;
		case GLFW_KEY_RIGHT_ALT: K = KeyboardDataComponent::KEY_RIGHT_ALT; break;
		case GLFW_KEY_MENU: K = KeyboardDataComponent::KEY_MENU; break;

		default: break;
		}

		// release, pressed, or repeated?
		switch (Action) {
		case GLFW_PRESS: S = KeyboardDataComponent::KEYSTATE_PRESSED; break;
		case GLFW_REPEAT: S = KeyboardDataComponent::KEYSTATE_REPEATED; break;
		case GLFW_RELEASE: S = KeyboardDataComponent::KEYSTATE_RELEASED; break;
		default: S = KeyboardDataComponent::KEYSTATE_OFF; break;
		}

		auto pKeyboardSys = SystemManager::getInstance()->getSystem<KeyboardInputSystem>();
		auto pInputEntityEntry = pKeyboardSys->m_handleToEntityMap.find(pWin);
		if (pKeyboardSys->m_handleToEntityMap.end() != pInputEntityEntry) {
			for (auto pEntity : pInputEntityEntry->second) {
				pEntity->getKeyboardDataComponent()->keyState(K) = S;	
			}
		}
		

	}
	void KeyboardInputSystem::characterCallbackFunc(struct GLFWwindow* pWin, unsigned int Codepoint) {

	}

}