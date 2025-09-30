#include "TriangleMeshEntity.h"

namespace crossforge {

	TriangleMeshEntity::TriangleMeshEntity(uint8_t componentBitmask): EntityBase(TriangleMeshEntity::identification) {
		m_inheritance.push_back(TriangleMeshEntity::identification);
		initialize(componentBitmask);
	}
	TriangleMeshEntity::~TriangleMeshEntity() {
		clear();
	}

	void TriangleMeshEntity::initialize(uint8_t componentBitmask) {
		clear();
		if (componentBitmask & POSITION_DATA_COMPONENT) addComponent(std::make_shared<PositionDataComponent>());
		if (componentBitmask & NORMAL_DATA_COMPONENT) addComponent(std::make_shared<NormalDataComponent>());
		if (componentBitmask & MESH_DEFINITIONS_COMPONENT) addComponent(std::make_shared<MeshDefinitionsComponent>());
		if (componentBitmask & TEXTURE_COORDINATES_COMPONENT) addComponent(std::make_shared<TextureCoordinateComponent>());
		if (componentBitmask & MATERIAL_DATA_COMPONENT) addComponent(std::make_shared<MaterialDataComponent>());
	}
	void TriangleMeshEntity::clear() {
		m_componentMap.clear();
	}

	PositionDataComponentPtr TriangleMeshEntity::getPositionDataComponent() {
		return this->getComponent<PositionDataComponent>();
	}
	NormalDataComponentPtr TriangleMeshEntity::getNormalDataComponent() {
		return this->getComponent<NormalDataComponent>();
	}
	MeshDefinitionsComponentPtr TriangleMeshEntity::getMeshDefinitionsComponent() {
		return this->getComponent<MeshDefinitionsComponent>();
	}

	TextureCoordinateComponentPtr TriangleMeshEntity::getTextureCoordinatesComponent() {
		return this->getComponent<TextureCoordinateComponent>();
	}
	MaterialDataComponentPtr TriangleMeshEntity::getMaterialDataComponent() {
		return this->getComponent<MaterialDataComponent>();
	}

}