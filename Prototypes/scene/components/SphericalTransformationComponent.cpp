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

	void SphericalTransformationComponent::initialize(const std::shared_ptr<const SphericalTransformationComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_origin = pRef->m_origin;
			m_rho = pRef->m_rho;
			m_theta = pRef->m_theta;
			m_phi = pRef->m_phi;
		}
	}
	void SphericalTransformationComponent::clear() {
		m_origin = Eigen::Vector3f::Zero();
		m_rho = 0.0f;
		m_theta = 0.0f;
		m_phi = 0.0f;
	}

	const Eigen::Vector3f SphericalTransformationComponent::getPosition() const {
		Eigen::Vector3f result = m_origin;

		const float theta = std::clamp(m_theta, 0.05f, float(EIGEN_PI - 0.05f));

		result.x() += m_rho * std::sin(theta) * std::cos(m_phi);
		result.y() += m_rho * std::cos(theta);
		result.z() += m_rho * std::sin(theta) * std::sin(m_phi);
		return result;
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

	/* Getters */
	const Eigen::Vector3f SphericalTransformationComponent::getOrigin()const {
		return m_origin;
	}
	const float SphericalTransformationComponent::getRho()const {
		return m_rho;
	}
	const float SphericalTransformationComponent::getTheta()const {
		return m_theta;
	}
	const float SphericalTransformationComponent::getPhi()const {
		return m_phi;
	}

	/* Setters */
	void SphericalTransformationComponent::setOrigin(const Eigen::Vector3f origin) {
		m_origin = origin;
	}
	void SphericalTransformationComponent::setRho(const float rho) {
		m_rho = rho;
	}
	void SphericalTransformationComponent::setTheta(const float theta) {
		m_theta = theta;
	}
	void SphericalTransformationComponent::setPhi(const float phi) {
		m_phi = phi;
	}

}