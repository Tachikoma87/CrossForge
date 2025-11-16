#include "SphericalTransformationComponent.h"

namespace crossforge {

	SphericalTransformationComponent::SphericalTransformationComponent() : ComponentBase(SphericalTransformationComponent::identification) {
		initialize();
	}
	SphericalTransformationComponent::SphericalTransformationComponent(const std::string childIdentification): ComponentBase(SphericalTransformationComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}
	SphericalTransformationComponent::~SphericalTransformationComponent() {

	}

	void SphericalTransformationComponent::initialize() {
		clear();
	}
	void SphericalTransformationComponent::clear() {
		m_origin = Eigen::Vector3f::Zero();
		m_rho = 0.0f;
		m_theta = 0.0f;
		m_phi = 0.0f;
	}

	Eigen::Vector3f& SphericalTransformationComponent::origin() {
		return m_origin;
	}
	float& SphericalTransformationComponent::rho() {
		return m_rho;
	}
	float& SphericalTransformationComponent::theta() {
		return m_theta;
	}
	float& SphericalTransformationComponent::phi() {
		return m_phi;
	}

	Eigen::Vector3f SphericalTransformationComponent::getPosition() {
		Eigen::Vector3f result = m_origin;

		m_theta = std::clamp(m_theta, 0.05f, float(EIGEN_PI-0.05f));

		result.x() += m_rho * std::sin(m_theta) * std::cos(m_phi);
		result.y() += m_rho * std::cos(m_theta);
		result.z() += m_rho * std::sin(m_theta) * std::sin(m_phi);
		return result;
	}
}