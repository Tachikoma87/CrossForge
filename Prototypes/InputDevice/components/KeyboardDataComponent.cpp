#include <crossforge/utility/GeneralUtility.hpp>
#include "KeyboardDataComponent.h"

namespace crossforge {

	KeyboardDataComponent::KeyboardDataComponent(): ComponentBase(KeyboardDataComponent::identification) {
		m_inheritance.push_back(KeyboardDataComponent::identification);
		initialize();
	}

	KeyboardDataComponent::~KeyboardDataComponent() {

	}

	void KeyboardDataComponent::initialize() {
		clear();
	}
	void KeyboardDataComponent::clear() {
		GeneralUtility::memset(m_keyStates, KeyState::KEYSTATE_OFF, KEY_COUNT);
	}

	KeyboardDataComponent::KeyState& KeyboardDataComponent::keyState(const Key key) {
		if (key < 0 || key >= KEY_COUNT) throw IndexOutOfBoundsExcept("key");
		return m_keyStates[key];
	}
	bool KeyboardDataComponent::isKeyRelease(const Key key)const {
		if (key < 0 || key >= KEY_COUNT) throw IndexOutOfBoundsExcept("key");
		return (KEYSTATE_RELEASED == m_keyStates[key]);
	}
	bool KeyboardDataComponent::isKeyPressed(const Key key)const {
		if (key < 0 || key >= KEY_COUNT) throw IndexOutOfBoundsExcept("key");
		return (KEYSTATE_PRESSED == m_keyStates[key]);
	}
}