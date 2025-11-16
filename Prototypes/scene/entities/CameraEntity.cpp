#include "CameraEntity.h"

namespace crossforge {

	CameraEntity::CameraEntity(uint8_t componentsBitmask) : SceneObjectEntity(CameraEntity::identification) {
		initialize(componentsBitmask);
	}
	CameraEntity::CameraEntity(const std::string childIdentification): SceneObjectEntity(CameraEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}
	CameraEntity::~CameraEntity() {
		clear();
	}

	void CameraEntity::initialize(uint8_t componentsBitmask) {
		clear();
		if (componentsBitmask & COMPONENT_UBO_CAMERA_DATA)		addComponent(std::make_shared<UBOCameraDataComponent>());
		if (componentsBitmask & COMPONENT_TRANSFORMATION_3D)	addComponent(std::make_shared<Transformation3DComponent>());
		if (componentsBitmask & COMPONENT_CAMERA_PROPERTIES)	addComponent(std::make_shared<CameraPropertiesComponent>());
		if (componentsBitmask & COMPONENT_TARGET_OBJECT)			addComponent(std::make_shared<TargetObjectComponent>());
	}
	void CameraEntity::clear() {
		m_componentMap.clear();
	}

	UBOCameraDataComponentPtr CameraEntity::getUboCameraDataComponent() {
		return getComponent<UBOCameraDataComponent>();
	}
	CameraPropertiesComponentPtr CameraEntity::getCameraPropertiesComponent() {
		return getComponent<CameraPropertiesComponent>();
	}
	TargetObjectComponentPtr CameraEntity::getTargetObjectComponentPtr() {
		return getComponent<TargetObjectComponent>();
	}
}