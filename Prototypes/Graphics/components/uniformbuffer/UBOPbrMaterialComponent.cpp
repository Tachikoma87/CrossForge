#include "UBOPbrMaterialComponent.h"

namespace crossforge {


	UBOPbrMaterialComponent::UBOPbrMaterialComponent(): UniformBufferComponent(UBOPbrMaterialComponent::identification) {
		clear();
	}
	UBOPbrMaterialComponent::~UBOPbrMaterialComponent() {
		clear();
	}
	UBOPbrMaterialComponent::UBOPbrMaterialComponent(const std::string childIdentification): UniformBufferComponent(UBOPbrMaterialComponent::identification) {
		m_inheritance.push_back(childIdentification);
		clear();
	}

	void UBOPbrMaterialComponent::initialize() {
		m_colorOffset = 0;
		m_metallicOffset = sizeof(float) * 4;
		m_roughnessOffset = sizeof(float) * 5;
		m_ambientOcclusionOffset = sizeof(float) * 6;

		try {
			const uint32_t bufferSize = sizeof(float) * 8;
			if(!UniformBufferComponent::initialize(bufferSize)) LogError("Initializing uniform buffer with size of " + std::to_string(bufferSize) + " failed.");
		}
		catch (CrossForgeException& e) {
			Logger::logException(e);
		}
		catch (std::exception& e) {
			LogError("Exception occurred while initializing uniform buffer: " + std::string(e.what()));
		}

	}
	void UBOPbrMaterialComponent::clear() {
		UniformBufferComponent::clear();
		m_colorOffset = 0;
		m_metallicOffset = 0;
		m_roughnessOffset = 0;
		m_ambientOcclusionOffset = 0;
	}

	void UBOPbrMaterialComponent::setColor(const Eigen::Vector4f color) {
		if (0 == m_bufferSize) throw NotInitializedExcept("Uniform buffer");
		setSubData(color.data(), sizeof(float) * 4, m_colorOffset);
	}
	void UBOPbrMaterialComponent::setMetallic(const float metallic) {
		if (0 == m_bufferSize) throw NotInitializedExcept("Uniform buffer");
		setSubData(&metallic, sizeof(float), m_metallicOffset);
	}
	void UBOPbrMaterialComponent::setRougness(const float roughness) {
		if (0 == m_bufferSize) throw NotInitializedExcept("Uniform buffer");
		setSubData(&roughness, sizeof(float), m_roughnessOffset);

	}
	void UBOPbrMaterialComponent::setAmbientOcclusion(const float ambientOcclusion) {
		if (0 == m_bufferSize) throw NotInitializedExcept("Uniform buffer");
		setSubData(&ambientOcclusion, sizeof(float), m_ambientOcclusionOffset);
	}
}