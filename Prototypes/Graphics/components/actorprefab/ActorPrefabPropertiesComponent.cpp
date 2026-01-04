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
		if(this != pRef.get()) clear();
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

	const bool ActorPrefabPropertiesComponent::getPropertyNormalMapping()const {
		return m_propertyNormalMapping;
	}
	const bool ActorPrefabPropertiesComponent::getPropertySkeletalAnimation()const {
		return m_propertySkeletalAnimation;
	}
	const bool ActorPrefabPropertiesComponent::getPropertyMorphTargetAnimation()const {
		return m_propertyMorphTargetAnimation;
	}

	void ActorPrefabPropertiesComponent::setPropertyNormalMapping(const bool setting) {
		m_propertyNormalMapping = setting;
	}
	void ActorPrefabPropertiesComponent::setPropertySkeletalAnimation(const bool setting) {
		m_propertySkeletalAnimation = setting;
	}
	void ActorPrefabPropertiesComponent::setPropertyMorphTargetAnimation(const bool setting) {
		m_propertyMorphTargetAnimation = setting;
	}
}