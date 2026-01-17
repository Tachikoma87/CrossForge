#include "UBOCameraDataComponent.h"

namespace crossforge {

	UBOCameraDataComponent::UBOCameraDataComponent(): UboBaseComponent(UBOCameraDataComponent::identification) {
		initialize();
	}
	UBOCameraDataComponent::UBOCameraDataComponent(const std::string childIdentification): UboBaseComponent(UBOCameraDataComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}
	UBOCameraDataComponent::~UBOCameraDataComponent() {

	}

	void UBOCameraDataComponent::initialize() {
		m_cameraMatrixOffset = 0;
		m_projectionMatrixOffset = sizeof(float) * 16;
		m_cameraPositionOffset = 2 * sizeof(float) * 16;
		UboBaseComponent::initialize( (16 + 16 + 4) * sizeof(float));

		setCameraMatrix(Eigen::Matrix4f::Identity());
		setProjectionMatrix(Eigen::Matrix4f::Identity());
		setCameraPosition(Eigen::Vector3f::Zero());
	}

	void UBOCameraDataComponent::setCameraMatrix(const Eigen::Matrix4f cameraMatrix) {
		setSubData(cameraMatrix.data(), sizeof(float) * 16, m_cameraMatrixOffset);
	}
	void UBOCameraDataComponent::setProjectionMatrix(const Eigen::Matrix4f projectionMatrix) {
		setSubData(projectionMatrix.data(), sizeof(float) * 16, m_projectionMatrixOffset);
	}
	void UBOCameraDataComponent::setCameraPosition(const Eigen::Vector3f position) {
		setSubData(position.data(), sizeof(float) * 3, m_cameraPositionOffset);
	}

}