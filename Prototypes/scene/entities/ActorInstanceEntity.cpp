#include "ActorInstanceEntity.h"

namespace crossforge {

	ActorInstanceEntity::ActorInstanceEntity(): SceneObjectEntity(ActorInstanceEntity::identification) {
		initialize();
	}
	ActorInstanceEntity::ActorInstanceEntity(const std::string childIdentification): SceneObjectEntity(ActorInstanceEntity::identification) {
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

	UBOTransformationDataComponentPtr ActorInstanceEntity::getUboTransformationDataComponent(const bool createIfNotExists) {
		return getComponent<UBOTransformationDataComponent>(createIfNotExists);
	}

}