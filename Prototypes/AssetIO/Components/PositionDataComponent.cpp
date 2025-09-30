#include "PositionDataComponent.h"

namespace crossforge {

	PositionDataComponent::PositionDataComponent(): ComponentBase(PositionDataComponent::identification) {

	}
	PositionDataComponent::~PositionDataComponent() {
		clear();
	}

	void PositionDataComponent::initialize(std::vector<Eigen::Vector3f> positions) {
		m_positions = positions;
	}
	void PositionDataComponent::clear() {
		m_positions.clear();
	}

	Eigen::Vector3f PositionDataComponent::operator[](const uint32_t index)const {
		return getPosition(index);
	}
	Eigen::Vector3f& PositionDataComponent::operator[](const uint32_t index) {
		return getPosition(index);
	}

	void PositionDataComponent::setPositions(std::vector<Eigen::Vector3f> positions) {
		m_positions = positions;
	}
	void PositionDataComponent::setPosition(Eigen::Vector3f position, uint32_t index) {
		if (index >= getPositionCount()) throw IndexOutOfBoundsExcept("index");
		m_positions[index] = position;
	}
	std::vector<Eigen::Vector3f> PositionDataComponent::getPositions()const {
		return m_positions;
	}
	std::vector<Eigen::Vector3f>& PositionDataComponent::getPositions() {
		return m_positions;
	}
	Eigen::Vector3f PositionDataComponent::getPosition(uint32_t index)const {
		if (index >= getPositionCount()) throw IndexOutOfBoundsExcept("index");
		return m_positions[index];
	}
	Eigen::Vector3f& PositionDataComponent::getPosition(uint32_t index) {
		if (index > getPositionCount()) throw IndexOutOfBoundsExcept("index");
		return m_positions[index];
	}

	uint32_t PositionDataComponent::getPositionCount()const {
		return m_positions.size();
	}
}