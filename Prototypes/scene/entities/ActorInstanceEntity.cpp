#include "ActorInstanceEntity.h"

namespace crossforge {

	ActorInstanceEntity::ActorInstanceEntity(uint8_t componentsBitmask): SceneObjectEntity(ActorInstanceEntity::identification) {
		initialize(componentsBitmask);
	}
	ActorInstanceEntity::ActorInstanceEntity(const std::string childIdentification): SceneObjectEntity(ActorInstanceEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}
	ActorInstanceEntity::~ActorInstanceEntity() {
		clear();
	}

	void ActorInstanceEntity::initialize(uint8_t componentsBitmask) {
		clear();
		if (componentsBitmask & COMPONENT_TRANSFORMATION_3D) addComponent(std::make_shared<Transformation3DComponent>());
		if (componentsBitmask & COMPONENT_UBO_TRANSFORMATION_DATA) addComponent(std::make_shared<UBOTransformationDataComponent>());
		if (componentsBitmask & COMPONENT_MOVEMENT_3D) addComponent(std::make_shared<Movement3DComponent>());
	}
	void ActorInstanceEntity::clear() {
		m_componentMap.clear();
	}


	UBOTransformationDataComponentPtr ActorInstanceEntity::getUboTransformationDataComponent() {
		return getComponent<UBOTransformationDataComponent>();
	}

}