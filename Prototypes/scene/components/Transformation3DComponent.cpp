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

	void Transformation3DComponent::initialize(const std::shared_ptr<const Transformation3DComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_localPosition = pRef->m_localPosition;
			m_localRotation = pRef->m_localRotation;
			m_localScale = pRef->m_localScale;
			m_globalPosition = pRef->m_globalPosition;
			m_globalRotation = pRef->m_globalRotation;
			m_globalScale = pRef->m_globalScale;
		}
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

	/* Getter */
	const Eigen::Vector3f Transformation3DComponent::getLocalPosition()const {
		return m_localPosition;
	}
	const Eigen::Vector3f Transformation3DComponent::getGlobalPosition()const {
		return m_globalPosition;
	}
	const Eigen::Quaternionf Transformation3DComponent::getLocalRotation()const {
		return m_localRotation;
	}
	const Eigen::Quaternionf Transformation3DComponent::getGlobalRotation()const {
		return m_globalRotation;
	}
	const Eigen::Vector3f Transformation3DComponent::getLocalScale()const {
		return m_localScale;
	}
	const Eigen::Vector3f Transformation3DComponent::getGlobalScale()const {
		return m_globalScale;
	}

	/** Setter */
	void Transformation3DComponent::setLocalPosition(const Eigen::Vector3f position) {
		m_localPosition = position;
	}
	void Transformation3DComponent::setGlobalPosition(const Eigen::Vector3f position) {
		m_globalPosition = position;
	}
	void Transformation3DComponent::setLocalRotation(const Eigen::Quaternionf rotation) {
		m_localRotation = rotation;
	}
	void Transformation3DComponent::setGlobalRotation(const Eigen::Quaternionf rotation) {
		m_globalRotation = rotation;
	}
	void Transformation3DComponent::setLocalScale(const Eigen::Vector3f scale) {
		m_localScale = scale;
	}
	void Transformation3DComponent::setGlobalScale(const Eigen::Vector3f scale) {
		m_globalScale = scale;
	}
}