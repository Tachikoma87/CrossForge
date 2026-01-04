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

	void CameraPropertiesComponent::initialize(const std::shared_ptr<const CameraPropertiesComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_projectionMatrix = pRef->m_projectionMatrix;
			m_viewMatrix = pRef->m_viewMatrix;
			m_cameraType = pRef->m_cameraType;
		}
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

	/** Getter **/
	const Eigen::Matrix4f CameraPropertiesComponent::getProjectionMatrix()const {
		return m_projectionMatrix;
	}
	const Eigen::Matrix4f CameraPropertiesComponent::getViewMatrix()const {
		return m_viewMatrix;
	}
	const CameraPropertiesComponent::CameraType CameraPropertiesComponent::getCameraType()const {
		return m_cameraType;
	}

	/** Setter **/
	void CameraPropertiesComponent::setProjectionMatrix(const Eigen::Matrix4f projectionMatrix) {
		m_projectionMatrix = projectionMatrix;
	}
	void CameraPropertiesComponent::setViewMatrix(const Eigen::Matrix4f viewMatrix) {
		m_viewMatrix = viewMatrix;
	}
	void CameraPropertiesComponent::setCameraType(const CameraType type) {
		m_cameraType = type;
	}

}