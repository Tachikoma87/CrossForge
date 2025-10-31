#include "UBODirectionalLightsComponent.h"

namespace crossforge {

	UBODirectionalLightsComponent::UBODirectionalLightsComponent(): UniformBufferComponent(UBODirectionalLightsComponent::identification) {
		m_lightCount = 0;
	}
	UBODirectionalLightsComponent::~UBODirectionalLightsComponent() {
		clear();
	}
	UBODirectionalLightsComponent::UBODirectionalLightsComponent(const std::string childIdentification):UniformBufferComponent(UBODirectionalLightsComponent::identification) {
		m_inheritance.push_back(childIdentification);
		m_lightCount = 0;
	}
	bool UBODirectionalLightsComponent::initialize(const uint32_t lightCount) {
		bool result = false;
		if (0 == lightCount) {
			LogError("Invalid light count of 0 specified. Buffer can not be initialized.");
			return result;
		}
		else if (lightCount > 50) {
			LogWarning("Directional light count of " + std::to_string(lightCount) + " specified. Are you sure this is correct?");
		}
		clear();
		uint32_t bufferSize = 0;
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_directionOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 4;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_colorOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 4;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_lightSpaceMatrixOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 16;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_shadowIdOffsets.push_back(bufferSize);
			bufferSize += sizeof(uint32_t) * 4;
		}

		try {
			if (!UniformBufferComponent::initialize(bufferSize)) LogError("Initialization of uniform buffer with size " + std::to_string(bufferSize) + " failed.");
			else {
				result = true;
				m_lightCount = lightCount;
			}
		}
		catch (CrossForgeException& e) {
			Logger::logException(e);
		}
		catch (std::exception &e) {
			LogError("Exception occurred during initialization of uniform buffer: " + std::string(e.what()));
		}
		return result;
	}
	void UBODirectionalLightsComponent::clear() {
		UniformBufferComponent::clear();
		m_lightCount = 0;
		m_directionOffsets.clear();
		m_colorOffsets.clear();
		m_lightSpaceMatrixOffsets.clear();
		m_shadowIdOffsets.clear();
	}

	uint32_t UBODirectionalLightsComponent::getLightCount()const {
		return m_lightCount;
	}

	void UBODirectionalLightsComponent::setDirection(const Eigen::Vector3f direction, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(direction.data(), sizeof(float) * 3, m_directionOffsets[lightIndex]);

	}
	void UBODirectionalLightsComponent::setColor(const Eigen::Vector4f color, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(color.data(), sizeof(float) * 4, m_colorOffsets[lightIndex]);

	}
	void UBODirectionalLightsComponent::setLightSpaceMatrix(const Eigen::Matrix4f lighSpaceMatrix, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(lighSpaceMatrix.data(), sizeof(float) * 16, m_lightSpaceMatrixOffsets[lightIndex]);
	}
	void UBODirectionalLightsComponent::setShadowId(int32_t shadowId, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(&shadowId, sizeof(int32_t), m_shadowIdOffsets[lightIndex]);
	}

}