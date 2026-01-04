#include "ActorPrefabEntity.h"

namespace crossforge {

	ActorPrefabEntity::ActorPrefabEntity(): EntityBase(ActorPrefabEntity::identification) {
		initialize(); 
	}
	ActorPrefabEntity::~ActorPrefabEntity() {
		clear();
	}

	ActorPrefabEntity::ActorPrefabEntity(const std::string childIdentification): EntityBase(ActorPrefabEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void ActorPrefabEntity::initialize() {
		clear();
	}
	void ActorPrefabEntity::clear() {
		m_componentMap.clear();
	}

	VertexBufferComponentPtr ActorPrefabEntity::getVertexBufferComponent(bool createIfNotExists) {
		return getComponent<VertexBufferComponent>(createIfNotExists);
	}
	IndexBufferComponentPtr ActorPrefabEntity::getIndexBufferComponent(bool createIfNotExists) {
		return getComponent<IndexBufferComponent>(createIfNotExists);
	}
	RenderGroupsComponentPtr ActorPrefabEntity::getRenderGroupsComponent(bool createIfNotExists) {
		return getComponent<RenderGroupsComponent>(createIfNotExists);
	}
	PbrMaterialsComponentPtr ActorPrefabEntity::getPBRMaterialsComponent(bool createIfNotExists) {
		return getComponent<PbrMaterialsComponent>(createIfNotExists);
	}
	VertexArrayComponentPtr ActorPrefabEntity::getVertexArrayComponent(bool createIfNotExists) {
		return getComponent<VertexArrayComponent>(createIfNotExists);
	}
	ActorPrefabPropertiesComponentPtr ActorPrefabEntity::getActorPrefabPropertiesComponent(bool createIfNotExists) {
		return getComponent<ActorPrefabPropertiesComponent>(createIfNotExists);
	}
}