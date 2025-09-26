#include "TriangleMeshEntity.h"

namespace CForge {

	TriangleMeshEntity::TriangleMeshEntity(): EntityBase(TriangleMeshEntity::identification) {
		
	}
	TriangleMeshEntity::~TriangleMeshEntity() {
		clear();
	}

	void TriangleMeshEntity::initialize() {
		
	}
	void TriangleMeshEntity::clear() {

	}

	PositionDataComponentPtr TriangleMeshEntity::getPositionDataComponent() {
		return this->getComponent<PositionDataComponent>(PositionDataComponent::identification);
	}
	NormalDataComponentPtr TriangleMeshEntity::getNormalDataComponent() {
		return this->getComponent<NormalDataComponent>(NormalDataComponent::identification);
	}
	MeshDefinitionsComponentPtr TriangleMeshEntity::getMeshDefinitionsComponent() {
		return this->getComponent<MeshDefinitionsComponent>(MeshDefinitionsComponent::identification);
	}

	TextureCoordinateComponentPtr TriangleMeshEntity::getTextureCoordinatesComponent() {
		return this->getComponent<TextureCoordinateComponent>(TextureCoordinateComponent::identification);
	}

}