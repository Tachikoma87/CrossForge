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

	void TargetObjectComponent::initialize() {
		clear();
	}
	void TargetObjectComponent::clear() {
		m_pTargetEntity = nullptr;
	}

	SceneObjectEntityPtr& TargetObjectComponent::targetSceneObject() {
		return m_pTargetEntity;
	}
}