#include "PositionComponent2D.h"

namespace crossforge {

	PositionComponent2D::PositionComponent2D(const Eigen::Vector2f pos): ComponentBase(PositionComponent2D::identification) {
		m_position = pos;
	}

	PositionComponent2D::~PositionComponent2D() {

	}

	Eigen::Vector2f PositionComponent2D::getPosition()const {
		return m_position;
	}

	Eigen::Vector2f& PositionComponent2D::getPosition() {
		return m_position;
	}

	void PositionComponent2D::setPosition(const Eigen::Vector2f pos) {
		m_position = pos;
	}

}