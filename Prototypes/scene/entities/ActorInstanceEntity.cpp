#include "ActorInstanceEntity.h"

namespace crossforge {

	ActorInstanceEntity::ActorInstanceEntity(): SceneNodeEntity(ActorInstanceEntity::identification) {
		initialize();
	}
	ActorInstanceEntity::ActorInstanceEntity(const std::string childIdentification): SceneNodeEntity(ActorInstanceEntity::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}
	ActorInstanceEntity::~ActorInstanceEntity() {
		clear();
	}

	void ActorInstanceEntity::initialize() {
		clear();
	}
		
	void ActorInstanceEntity::clear() {
		m_componentMap.clear();
	}

	UboTransformationDataComponentPtr ActorInstanceEntity::getUboTransformationDataComponent(const bool createIfNotExists) {
		return getComponent<UboTransformationDataComponent>(createIfNotExists);
	}

}