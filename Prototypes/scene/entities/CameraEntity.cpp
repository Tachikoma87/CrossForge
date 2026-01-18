#include "CameraEntity.h"

namespace crossforge {

	CameraEntity::CameraEntity() : SceneNodeEntity(CameraEntity::identification) {
		initialize();
	}
	CameraEntity::CameraEntity(const std::string childIdentification): SceneNodeEntity(CameraEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}
	CameraEntity::~CameraEntity() {
		clear();
	}

	void CameraEntity::initialize() {
		clear();
	}
	void CameraEntity::clear() {
		m_componentMap.clear();
	}

	UboCameraDataComponentPtr CameraEntity::getUboCameraDataComponent(const bool createIfNotExists) {
		return getComponent<UboCameraDataComponent>(createIfNotExists);
	}
	CameraPropertiesComponentPtr CameraEntity::getCameraPropertiesComponent(const bool createIfNotExists) {
		return getComponent<CameraPropertiesComponent>(createIfNotExists);
	}
	TargetSceneNodeComponentPtr CameraEntity::getTargetObjectComponent(const bool createIfNotExists) {
		return getComponent<TargetSceneNodeComponent>(createIfNotExists);
	}
}