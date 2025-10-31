#include "Movement3DComponent.h"

namespace crossforge {

	Movement3DComponent::Movement3DComponent(): ComponentBase(Movement3DComponent::identification) {
		clear();
	}
	Movement3DComponent::~Movement3DComponent() {
		clear();
	}
	Movement3DComponent::Movement3DComponent(const std::string childIdentification): ComponentBase(Movement3DComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void Movement3DComponent::initialize() {
		clear();
	}
	void Movement3DComponent::clear() {
		m_positionDelta = Eigen::Vector3f::Zero();
		m_rotationDetla = Eigen::Quaternionf::Identity();
		m_scaleDelta = Eigen::Vector3f::Zero();
	}

	Eigen::Vector3f& Movement3DComponent::positionDelta() {
		return m_positionDelta;
	}
	Eigen::Quaternionf& Movement3DComponent::rotationDelta() {
		return m_rotationDetla;
	}
	Eigen::Vector3f& Movement3DComponent::scaleDelta() {
		return m_scaleDelta;
	}
}