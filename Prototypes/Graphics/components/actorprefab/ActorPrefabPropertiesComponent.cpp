#include "ActorPrefabPropertiesComponent.h"

namespace crossforge {

	ActorPrefabPropertiesComponent::ActorPrefabPropertiesComponent(): ComponentBase(ActorPrefabPropertiesComponent::identification) {
		initialize();
	}
	ActorPrefabPropertiesComponent::~ActorPrefabPropertiesComponent() {
		clear();

	}
	ActorPrefabPropertiesComponent::ActorPrefabPropertiesComponent(const std::string childIdentification): ComponentBase(ActorPrefabPropertiesComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void ActorPrefabPropertiesComponent::initialize(std::shared_ptr<ActorPrefabPropertiesComponent> pRef) {
		clear();
		if(nullptr != pRef){
			m_propertyNormalMapping = pRef->propertyNormalMapping();
			m_propertySkeletalAnimation = pRef->propertySkeltalAnimation();
			m_propertyMorphTargetAnimation = pRef->propertyMorphTargetAnimation();
		}
	}
	void ActorPrefabPropertiesComponent::clear() {
		m_propertyNormalMapping = false;
		m_propertySkeletalAnimation = false;
		m_propertyMorphTargetAnimation = false;
	}

	bool& ActorPrefabPropertiesComponent::propertyNormalMapping() {
		return m_propertyNormalMapping;
	}
	bool& ActorPrefabPropertiesComponent::propertySkeltalAnimation() {
		return m_propertySkeletalAnimation;
	}
	bool& ActorPrefabPropertiesComponent::propertyMorphTargetAnimation() {
		return m_propertyMorphTargetAnimation;
	}
}