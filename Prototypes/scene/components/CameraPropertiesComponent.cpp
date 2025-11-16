#include "CameraPropertiesComponent.h"

namespace crossforge {

	CameraPropertiesComponent::CameraPropertiesComponent(): ComponentBase(CameraPropertiesComponent::identification) {
		initialize();
	}
	CameraPropertiesComponent::CameraPropertiesComponent(const std::string childIdentification): ComponentBase(CameraPropertiesComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
		
	}
	CameraPropertiesComponent::~CameraPropertiesComponent() {
		clear();
	}

	void CameraPropertiesComponent::initialize() {
		clear();
	}
	void CameraPropertiesComponent::clear() {
		m_projectionMatrix = Eigen::Matrix4f::Identity();
		m_viewMatrix = Eigen::Matrix4f::Identity();
		m_cameraType = CameraType::UNKNOWN;
	}

	Eigen::Matrix4f& CameraPropertiesComponent::projectionMatrix() {
		return m_projectionMatrix;
	}

	Eigen::Matrix4f& CameraPropertiesComponent::viewMatrix() {
		return m_viewMatrix;
	}

	CameraPropertiesComponent::CameraType& CameraPropertiesComponent::cameraType() {
		return m_cameraType;
	}

}