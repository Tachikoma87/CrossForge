#include "CameraPropertiesComponent.h"

namespace crossforge {

	CameraPropertiesComponent::CameraPropertiesComponent(): ComponentBase(CameraPropertiesComponent::identification) {
		m_projectionMatrix = Eigen::Matrix4f::Identity();
		m_viewMatrix = Eigen::Matrix4f::Identity();
	}
	CameraPropertiesComponent::CameraPropertiesComponent(const std::string childIdentification): ComponentBase(CameraPropertiesComponent::identification) {
		m_inheritance.push_back(childIdentification);
		m_projectionMatrix = Eigen::Matrix4f::Identity();
		m_viewMatrix = Eigen::Matrix4f::Identity();
	}
	CameraPropertiesComponent::~CameraPropertiesComponent() {

	}

	Eigen::Matrix4f& CameraPropertiesComponent::projectionMatrix() {
		return m_projectionMatrix;
	}

	Eigen::Matrix4f& CameraPropertiesComponent::viewMatrix() {
		return m_viewMatrix;
	}

}