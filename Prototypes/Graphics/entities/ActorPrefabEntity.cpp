#include "ActorPrefabEntity.h"

namespace crossforge {

	ActorPrefabEntity::ActorPrefabEntity(uint8_t componentsBitmask): EntityBase(ActorPrefabEntity::identification) {
		initialize(componentsBitmask); 
	}
	ActorPrefabEntity::~ActorPrefabEntity() {
		clear();
	}

	ActorPrefabEntity::ActorPrefabEntity(const std::string childIdentification): EntityBase(ActorPrefabEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void ActorPrefabEntity::initialize(uint8_t componentsBitmask) {
		clear();
		if (componentsBitmask & VERTEX_BUFFER_COMPONENT) addComponent(std::make_shared<VertexBufferComponent>());
		if (componentsBitmask & INDEX_BUFFER_COMPONENT) addComponent(std::make_shared<IndexBufferComponent>());
		if (componentsBitmask & RENDER_GROUPS_COMPONENT) addComponent(std::make_shared<RenderGroupsComponent>());
		if (componentsBitmask & PBR_MATERIALS_COMPONENT) addComponent(std::make_shared<PbrMaterialsComponent>());
		if (componentsBitmask & VERTEX_ARRAY_COMPONENT) addComponent(std::make_shared<VertexArrayComponent>());
	}
	void ActorPrefabEntity::clear() {
		m_componentMap.clear();
	}

	VertexBufferComponentPtr ActorPrefabEntity::getVertexBufferComponent() {
		return getComponent<VertexBufferComponent>();
	}
	IndexBufferComponentPtr ActorPrefabEntity::getIndexBufferComponent() {
		return getComponent<IndexBufferComponent>();
	}
	RenderGroupsComponentPtr ActorPrefabEntity::getRenderGroupsComponent() {
		return getComponent<RenderGroupsComponent>();
	}
	PbrMaterialsComponentPtr ActorPrefabEntity::getPBRMaterialsComponent() {
		return getComponent<PbrMaterialsComponent>();
	}
	VertexArrayComponentPtr ActorPrefabEntity::getVertexArrayComponent() {
		return getComponent<VertexArrayComponent>();
	}
}