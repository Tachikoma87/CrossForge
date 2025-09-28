#include "InputDeviceEntity.h"

namespace crossforge {

	InputDeviceEntity::InputDeviceEntity(uint8_t componentBitMask): EntityBase(InputDeviceEntity::identificaiton) {
		m_inheritance.push_back(InputDeviceEntity::identificaiton);
		initialize(componentBitMask);
	}

	InputDeviceEntity::~InputDeviceEntity() {
		
	}

	void InputDeviceEntity::initialize(uint8_t componentBitMask) {
		m_componentMap.clear();
		if (componentBitMask & MOUSE_DATA_COMPONENT) addComponent(std::make_shared<MouseDataComponent>());
		if (componentBitMask & KEYBOARD_DATA_COMPONENT) addComponent(std::make_shared<KeyboardDataComponent>());
		if (componentBitMask & ASSOCIATED_WINDOW_COMPONENT) addComponent(std::make_shared<AssociatedWindowComponent>());
	}

	MouseDataComponentPtr InputDeviceEntity::getMouseDataComponent() {
		return getComponent<MouseDataComponent>();
	}
	KeyboardDataComponentPtr InputDeviceEntity::getKeyboardDataComponent() {
		return getComponent<KeyboardDataComponent>();
	}
	AssociatedWindowComponentPtr InputDeviceEntity::getAssociatedWindowComponent() {
		return getComponent<AssociatedWindowComponent>();
	}
}