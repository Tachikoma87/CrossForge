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

	Transformation3DComponentPtr SceneObjectEntity::getTransformation3DComponent() {
		return getComponent<Transformation3DComponent>();
	}
	Movement3DComponentPtr SceneObjectEntity::getMovement3DComponent() {
		return getComponent<Movement3DComponent>();
	}
	ChildObjectsComponentPtr SceneObjectEntity::getChildObjectsComponent() {
		return getComponent<ChildObjectsComponent>();
	}
}