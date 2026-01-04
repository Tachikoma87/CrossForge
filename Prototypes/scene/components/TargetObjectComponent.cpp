#include "TargetObjectComponent.h"

namespace crossforge {

	TargetObjectComponent::TargetObjectComponent(): ComponentBase(TargetObjectComponent::identification) {
		initialize();
	}
	TargetObjectComponent::TargetObjectComponent(const std::string childIdentification): ComponentBase(TargetObjectComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}
	TargetObjectComponent::~TargetObjectComponent() {
		clear();
	}

	void TargetObjectComponent::initialize(const std::shared_ptr<const TargetObjectComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_pTargetEntity = pRef->m_pTargetEntity;
		}
	}
	void TargetObjectComponent::clear() {
		m_pTargetEntity = nullptr;
	}

	SceneObjectEntityPtr& TargetObjectComponent::targetSceneObject() {
		return m_pTargetEntity;
	}

	const SceneObjectEntityCPtr TargetObjectComponent::getTargetSceneObject()const {
		return m_pTargetEntity;
	}
	void TargetObjectComponent::setTargetSceneObject(const SceneObjectEntityPtr pObj) {
		m_pTargetEntity = pObj;
	}
}