#include <crossforge/utility/GeneralUtility.hpp>
#include "MouseDataComponent.h"

namespace crossforge {

	MouseDataComponent::MouseDataComponent(): ComponentBase(MouseDataComponent::identification) {
		m_inheritance.push_back(MouseDataComponent::identification);
		GeneralUtility::memset(m_buttonStates, STATE_OFF, BUTTON_COUNT);
	}

	MouseDataComponent::~MouseDataComponent() {

	}

	Eigen::Vector2f& MouseDataComponent::position() {
		return m_position;
	}
	Eigen::Vector2f& MouseDataComponent::positionDelta() {
		return m_positionDelta;
	}
	Eigen::Vector2f& MouseDataComponent::wheelOffset() {
		return m_wheelOffset;
	}
	MouseDataComponent::ButtonState& MouseDataComponent::buttonState(const Button btn) {
		if (0 > btn || btn >= BUTTON_COUNT) throw IndexOutOfBoundsExcept("btn");
		return m_buttonStates[btn];
	}
}