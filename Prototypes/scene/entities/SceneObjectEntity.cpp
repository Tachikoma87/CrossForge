#include "SceneObjectEntity.h"
#include "../components/ChildObjectsComponent.h"

namespace crossforge {

	SceneObjectEntity::SceneObjectEntity(): EntityBase(SceneObjectEntity::identification) {

	}

	SceneObjectEntity::SceneObjectEntity(const std::string childIdentification) : EntityBase(SceneObjectEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}
	SceneObjectEntity::~SceneObjectEntity() {

	}

	Transformation3DComponentPtr SceneObjectEntity::getTransformation3DComponent(const bool createIfNotExists) {
		return getComponent<Transformation3DComponent>(createIfNotExists);
	}
	Movement3DComponentPtr SceneObjectEntity::getMovement3DComponent(const bool createIfNotExists) {
		return getComponent<Movement3DComponent>(createIfNotExists);
	}
	ChildObjectsComponentPtr SceneObjectEntity::getChildObjectsComponent(const bool createIfNotExists) {
		return getComponent<ChildObjectsComponent>(createIfNotExists);
	}
}