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

	void Movement3DComponent::initialize(const std::shared_ptr<const Movement3DComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_positionDelta = pRef->m_positionDelta;
			m_rotationDetla = pRef->m_rotationDetla;
			m_scaleDelta = pRef->m_scaleDelta;
		}
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

	/** Getter **/
	const Eigen::Vector3f Movement3DComponent::getPositionDelta()const {
		return m_positionDelta;
	}
	const Eigen::Quaternionf Movement3DComponent::getRotationDelta()const {
		return m_rotationDetla;
	}
	const Eigen::Vector3f Movement3DComponent::getScaleDelta()const {
		return m_scaleDelta;
	}

	/** Setter **/
	void Movement3DComponent::setPositionDelta(const Eigen::Vector3f positionDelta) {
		m_positionDelta = positionDelta;
	}
	void Movement3DComponent::setRotationDelta(const Eigen::Quaternionf rotationDelta) {
		m_rotationDetla = rotationDelta;
	}
	void Movement3DComponent::setScaleDelta(const Eigen::Vector3f scaleDelta) {
		m_scaleDelta = scaleDelta;
	}
}