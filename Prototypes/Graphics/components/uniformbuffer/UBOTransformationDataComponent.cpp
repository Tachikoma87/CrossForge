#include <glad/glad.h>
#include "UBOTransformationDataComponent.h"

namespace crossforge {

	UBOTransformationDataComponent::UBOTransformationDataComponent(): UniformBufferComponent(UBOTransformationDataComponent::identification) {
		initialize();
	}
	UBOTransformationDataComponent::~UBOTransformationDataComponent() {
		
	}
	UBOTransformationDataComponent::UBOTransformationDataComponent(const std::string childIdentification): UniformBufferComponent(UBOTransformationDataComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void UBOTransformationDataComponent::initialize() {
		m_modelMatrixOffset = 0;
		m_normalMatrixOffset = m_modelMatrixOffset + sizeof(float) * 16;
		UniformBufferComponent::initialize((16 + 12) * sizeof(float) );

		setModelMatrix(Eigen::Matrix4f::Identity());
		setNormalMatrix(Eigen::Matrix4f::Identity());
	}

	void UBOTransformationDataComponent::setModelMatrix(const Eigen::Matrix4f modelMatrix) {
		setSubData(modelMatrix.data(), sizeof(float) * 16, m_modelMatrixOffset);
	}
	void UBOTransformationDataComponent::setNormalMatrix(const Eigen::Matrix4f normalMatrix) {
		setSubData(normalMatrix.block<3,4>(0,0).data(), sizeof(float) * 12, m_normalMatrixOffset);
	}

}