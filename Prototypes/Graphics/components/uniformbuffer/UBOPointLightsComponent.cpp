#include "UBOPointLightsComponent.h"

namespace crossforge {

	UBOPointLightsComponent::UBOPointLightsComponent(): UniformBufferComponent(UBOPointLightsComponent::identification) {
		clear();
	}
	UBOPointLightsComponent::~UBOPointLightsComponent() {
		clear();
	}
	UBOPointLightsComponent::UBOPointLightsComponent(const std::string childIdentification): UniformBufferComponent(UBOPointLightsComponent::identification) {
		m_inheritance.push_back(childIdentification);
		clear();
	}

	bool UBOPointLightsComponent::initialize(uint32_t lightCount) {
		bool result = false;
		if (0 == lightCount) {
			LogError("Invalid light count of 0 specified. Can not initialize uniform buffer.");
			return result;
		}
		clear();
		
		uint32_t bufferSize = 0;
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_positionOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 4;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_colorOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 3;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_intensityOffsets.push_back(bufferSize);
			bufferSize += sizeof(float);
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_attenuationOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 4;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_directionOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 4;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_lightSpaceMatrixOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 16;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_shadowIndexOffsets.push_back(bufferSize);
			bufferSize += sizeof(int32_t) * 4;
		}

		try {
			if (!UniformBufferComponent::initialize(bufferSize)) LogError("Failed to initiaize uniform buffer with size of " + std::to_string(bufferSize) + " bytes.");
			else {
				result = true;
				m_lightCount = lightCount;
			}
		}
		catch (const CrossForgeException& e) {
			Logger::logException(e);
		}
		catch (std::exception& e) {
			LogError("Exception while initializing uniform buffer: " + std::string(e.what()));
		}
		return result;
	}
	void UBOPointLightsComponent::clear() {
		UniformBufferComponent::clear();
		m_lightCount = 0;
		m_positionOffsets.clear();
		m_colorOffsets.clear();
		m_intensityOffsets.clear();
		m_attenuationOffsets.clear();
		m_directionOffsets.clear();
		m_lightSpaceMatrixOffsets.clear();
		m_shadowIndexOffsets.clear();
	}

	void UBOPointLightsComponent::setPosition(const Eigen::Vector3f position, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(position.data(), sizeof(float) * 3, m_positionOffsets[lightIndex]);
	}
	void UBOPointLightsComponent::setColor(const Eigen::Vector3f color, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(color.data(), sizeof(float) * 3, m_colorOffsets[lightIndex]);
	}
	void UBOPointLightsComponent::setIntensity(const float intensity, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(&intensity, sizeof(float), m_intensityOffsets[lightIndex]);
	}
	void UBOPointLightsComponent::setDirection(const Eigen::Vector3f direction, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(direction.data(), sizeof(float) * 3, m_directionOffsets[lightIndex]);
	}
	void UBOPointLightsComponent::setLightSpaceMatrix(const Eigen::Matrix4f lightSpaceMatrix, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(lightSpaceMatrix.data(), sizeof(float) * 16, m_lightSpaceMatrixOffsets[lightIndex]);
	}
	void UBOPointLightsComponent::setShadowIndex(const int32_t shadowIndex, uint32_t lightIndex) {
		if (lightIndex >= m_lightCount) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(&shadowIndex, sizeof(int32_t), m_shadowIndexOffsets[lightIndex]);
	}
}
