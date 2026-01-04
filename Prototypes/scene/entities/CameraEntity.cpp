#include "CameraEntity.h"

namespace crossforge {

	CameraEntity::CameraEntity() : SceneObjectEntity(CameraEntity::identification) {
		initialize();
	}
	CameraEntity::CameraEntity(const std::string childIdentification): SceneObjectEntity(CameraEntity::identification) {
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

	UBOCameraDataComponentPtr CameraEntity::getUboCameraDataComponent(const bool createIfNotExists) {
		return getComponent<UBOCameraDataComponent>(createIfNotExists);
	}
	CameraPropertiesComponentPtr CameraEntity::getCameraPropertiesComponent(const bool createIfNotExists) {
		return getComponent<CameraPropertiesComponent>(createIfNotExists);
	}
	TargetObjectComponentPtr CameraEntity::getTargetObjectComponent(const bool createIfNotExists) {
		return getComponent<TargetObjectComponent>(createIfNotExists);
	}
}