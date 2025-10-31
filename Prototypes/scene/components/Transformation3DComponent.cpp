#include "Transformation3DComponent.h"

namespace crossforge {
	Transformation3DComponent::Transformation3DComponent(): ComponentBase(Transformation3DComponent::identification) {
		initialize();
	}
	Transformation3DComponent::~Transformation3DComponent() {
		clear();
	}
	Transformation3DComponent::Transformation3DComponent(const std::string childIdentification): ComponentBase(Transformation3DComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void Transformation3DComponent::initialize() {
		clear();
	}
	void Transformation3DComponent::clear() {
		m_localPosition = Eigen::Vector3f::Zero();
		m_localRotation = Eigen::Quaternionf::Identity();
		m_localScale = Eigen::Vector3f::Ones();
		m_globalPosition = m_localPosition;
		m_globalRotation = m_localRotation;
		m_globalScale = m_localScale;
	}

	Eigen::Vector3f& Transformation3DComponent::localPosition() {
		return m_localPosition;
	}
	Eigen::Vector3f& Transformation3DComponent::globalPosition() {
		return m_globalPosition;
	}
	Eigen::Quaternionf& Transformation3DComponent::localRotation() {
		return m_localRotation;
	}
	Eigen::Quaternionf& Transformation3DComponent::globalRotation() {
		return m_globalRotation;
	}
	Eigen::Vector3f& Transformation3DComponent::localScale() {
		return m_localScale;
	}
	Eigen::Vector3f& Transformation3DComponent::globalScale() {
		return m_globalScale;
	}
}