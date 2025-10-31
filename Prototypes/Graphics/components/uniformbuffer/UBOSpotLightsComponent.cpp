#include "UBOSpotLightsComponent.h"

namespace crossforge {

	UBOSpotLightsComponent::UBOSpotLightsComponent(): UniformBufferComponent(UBOSpotLightsComponent::identification) {
		clear();
	}
	UBOSpotLightsComponent::UBOSpotLightsComponent(const std::string childIdentification): UniformBufferComponent(UBOSpotLightsComponent::identification) {
		m_inheritance.push_back(childIdentification);
		clear();
	}
	UBOSpotLightsComponent::~UBOSpotLightsComponent() {
		clear();
	}

	bool UBOSpotLightsComponent::initialize(uint32_t lightCount) {
		bool result = false;
		if (0 == lightCount) {
			LogError("Invalid light count of 0 specified. Can not initialize uniform buffer.");
			return result;
		}

		uint32_t bufferSize = 0;
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_positionOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 4;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_directionOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 3;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_outerCutoffOffsets.push_back(bufferSize);
			bufferSize += sizeof(float);
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
			bufferSize += sizeof(float) * 3;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_innerCutoffOffsets.push_back(bufferSize);
			bufferSize += sizeof(float);
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_lightSpaceMatrixOffsets.push_back(bufferSize);
			bufferSize += sizeof(float) * 16;
		}
		for (uint32_t i = 0; i < lightCount; ++i) {
			m_shadowIdOffsets.push_back(bufferSize);
			bufferSize += sizeof(int32_t) * 4;
		}

		try {
			if (!UniformBufferComponent::initialize(bufferSize)) LogError("Failed to initialize uniform buffer with " + std::to_string(bufferSize) + " bytes.");
			else {
				result = true;
				m_lightCount = lightCount;
			}
		}
		catch (const CrossForgeException& e) {
			Logger::logException(e);
		}
		catch (std::exception& e) {
			LogError("Exception occurred while initializing uniform buffer: " + std::string(e.what()));
		}
		return result;

	}
	void UBOSpotLightsComponent::clear() {
		UniformBufferComponent::clear();

		m_lightCount = 0;
		m_positionOffsets.clear();
		m_directionOffsets.clear();
		m_outerCutoffOffsets.clear();
		m_colorOffsets.clear();
		m_intensityOffsets.clear();
		m_attenuationOffsets.clear();
		m_innerCutoffOffsets.clear();
		m_lightSpaceMatrixOffsets.clear();
		m_shadowIdOffsets.clear();
	}

	void UBOSpotLightsComponent::setPosition(const Eigen::Vector3f position, uint32_t lightIndex) {
		if (0 >= lightIndex) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(position.data(), sizeof(float) * 3, m_positionOffsets[lightIndex]);
	}
	void UBOSpotLightsComponent::setDirection(const Eigen::Vector3f direction, uint32_t lightIndex) {
		if (0 >= lightIndex) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(direction.data(), sizeof(float) * 3, m_directionOffsets[lightIndex]);
	}
	void UBOSpotLightsComponent::setOuterCutoff(const float outerCutoff, uint32_t lightIndex) {
		if (0 >= lightIndex) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(&outerCutoff, sizeof(float), m_outerCutoffOffsets[lightIndex]);
	}
	void UBOSpotLightsComponent::setColor(const Eigen::Vector3f color, uint32_t lightIndex) {
		if (0 >= lightIndex) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(color.data(), sizeof(float) * 3, m_colorOffsets[lightIndex]);
	}
	void UBOSpotLightsComponent::setIntensity(const float intensity, uint32_t lightIndex) {
		if (0 >= lightIndex) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(&intensity, sizeof(float), m_intensityOffsets[lightIndex]);
	}
	void UBOSpotLightsComponent::setAttenuation(const Eigen::Vector3f attenuation, uint32_t lightIndex) {
		if (0 >= lightIndex) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(attenuation.data(), sizeof(float) * 3, m_attenuationOffsets[lightIndex]);
	}
	void UBOSpotLightsComponent::setInnerCutoff(const float innerCutoff, uint32_t lightIndex) {
		if (0 >= lightIndex) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(&innerCutoff, sizeof(float), m_innerCutoffOffsets[lightIndex]);
	}
	void UBOSpotLightsComponent::setLightSpaceMatrix(const Eigen::Matrix4f lightSpaceMatrix, uint32_t lightIndex) {
		if (0 >= lightIndex) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(lightSpaceMatrix.data(), sizeof(float) * 16, m_lightSpaceMatrixOffsets[lightIndex]);
	}
	void UBOSpotLightsComponent::setShadowIndex(const int32_t shadowIndex, uint32_t lightIndex) {
		if (0 >= lightIndex) throw IndexOutOfBoundsExcept("lightIndex");
		setSubData(&shadowIndex, sizeof(int32_t), m_shadowIdOffsets[lightIndex]);
	}
}