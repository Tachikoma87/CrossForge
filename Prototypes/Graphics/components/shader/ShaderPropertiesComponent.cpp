#include "ShaderPropertiesComponent.h"

namespace crossforge {

	ShaderPropertiesComponent::ShaderPropertiesComponent() : ComponentBase(ShaderPropertiesComponent::identification) {
		initialize();
	}
	ShaderPropertiesComponent::~ShaderPropertiesComponent() {
		clear();
	}
	ShaderPropertiesComponent::ShaderPropertiesComponent(const std::string childIdentification) : ComponentBase(ShaderPropertiesComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void ShaderPropertiesComponent::initialize(std::shared_ptr<ShaderPropertiesComponent> pRef) {
		clear();
		m_featureNamesMap.insert(std::pair(SHADER_FEATURE_DIRECTIONAL_LIGHTS, "DIRECTIONAL_LIGHTS"));
		m_featureNamesMap.insert(std::pair(SHADER_FEATURE_POINT_LIGHTS, "POINT_LIGHTS"));
		m_featureNamesMap.insert(std::pair(SHADER_FEATURE_SPOT_LIGHTS, "SPOT_LIGHTS"));
		m_featureNamesMap.insert(std::pair(SHADER_FEATURE_MULTIPLE_SHADOWS, "MULTIPLE_SHADOWS"));
		m_featureNamesMap.insert(std::pair(SHADER_FEATURE_NORMAL_MAPPING, "NORMAL_MAPPING"));
		m_featureNamesMap.insert(std::pair(SHADER_FEATURE_PCF_SHADOWS, "PCF_SHADOWS"));
		m_featureNamesMap.insert(std::pair(SHADER_FEATURE_VERTEX_COLORS, "VERTEX_COLORS"));
		m_featureNamesMap.insert(std::pair(SHADER_FEATURE_SKELETAL_ANIMATION, "SKELETAL_ANIMATION"));
		m_featureNamesMap.insert(std::pair(SHADER_FEATURE_MORPH_TARGET_ANIMATION, "MORPH_TARGET_ANIMATION"));

		m_directionalLightsSizeConstDefinition		= "const uint DirLightSize";
		m_pointLightsSizeConstDefinition			= "const uint PointLightSize";
		m_spotLightsSizeConstDefinition				= "const uint SpotLightSize";
		m_activeDirectionalLightsConstDefinition	= "const uint ActiveDirLights";
		m_activePointLightsConstDefinition			= "const uint ActivePointights";
		m_activeSpotLightsConstDefinition			= "const uint ActiveSpotLights";

		if (nullptr != pRef) {
			m_directionalLightsSize = pRef->directionalLightsSize();
			m_pointLightsSize = pRef->pointLightsSize();
			m_spotLightsSize = pRef->spotLightsSize();
			m_shadowMapCount = pRef->shadowMapCount();

			m_activeDirectionalLightsCount = pRef->activeDirectionalLightsCount();
			m_activePointLightsCount = pRef->activePointLightsCount();
			m_activeSpotLightsCount = pRef->activeSpotLightsCount();
			m_featureMask = pRef->featureMask();

			m_directionalLightsSizeConstDefinition = pRef->directionalLightsSizeConstDefinition();
			m_pointLightsSizeConstDefinition = pRef->pointLightsSizeConstDefinition();
			m_spotLightsSizeConstDefinition = pRef->spotLightsSizeConstDefinition();
			m_activeDirectionalLightsConstDefinition = pRef->activeDirectionalLightsConstDefinition();
			m_activePointLightsConstDefinition = pRef->activePointLightsConstDefinition();
			m_activeSpotLightsConstDefinition = pRef->activeSpotLightsConstDefinition();
			

		}
	}

	void ShaderPropertiesComponent::clear() {
		m_directionalLightsSize = 0;
		m_pointLightsSize = 0;
		m_spotLightsSize = 0;
		m_shadowMapCount = 0;

		m_activeDirectionalLightsCount = 0;
		m_activePointLightsCount = 0;
		m_activeSpotLightsCount = 0;
		m_shadowMapCount = 0;
		m_featureMask = 0;
		m_featureNamesMap.clear();
	}

	uint32_t& ShaderPropertiesComponent::directionalLightsSize() {
		return m_directionalLightsSize;
	}
	uint32_t& ShaderPropertiesComponent::pointLightsSize() {
		return m_pointLightsSize;
	}
	uint32_t& ShaderPropertiesComponent::spotLightsSize() {
		return m_spotLightsSize;
	}
	uint32_t& ShaderPropertiesComponent::activeDirectionalLightsCount() {
		return m_activeDirectionalLightsCount;
	}
	uint32_t& ShaderPropertiesComponent::activePointLightsCount() {
		return m_activePointLightsCount;
	}
	uint32_t& ShaderPropertiesComponent::activeSpotLightsCount() {
		return m_activeSpotLightsCount;
	}

	uint32_t& ShaderPropertiesComponent::shadowMapCount() {
		return m_shadowMapCount;
	}

	bool ShaderPropertiesComponent::hasFeatures(uint16_t featureMask) {
		return m_featureMask & featureMask;
	}
	void ShaderPropertiesComponent::addFeatures(uint16_t featureMask) {
		m_featureMask |= featureMask;
	}
	void ShaderPropertiesComponent::removeFeatures(uint16_t featureMask) {
		m_featureMask = m_featureMask & (m_featureMask ^ featureMask);
	}

	uint16_t& ShaderPropertiesComponent::featureMask() {
		return m_featureMask;
	}

	std::string ShaderPropertiesComponent::getFeatureName(ShaderFeature feature)const {
		auto featureName = this->m_featureNamesMap.find(feature);
		if (m_featureNamesMap.end() == featureName) throw IndexOutOfBoundsExcept("feature");
		return featureName->second;
	}

	std::string& ShaderPropertiesComponent::directionalLightsSizeConstDefinition() {
		return m_directionalLightsSizeConstDefinition;
	}
	std::string& ShaderPropertiesComponent::pointLightsSizeConstDefinition() {
		return m_pointLightsSizeConstDefinition;
	}
	std::string& ShaderPropertiesComponent::spotLightsSizeConstDefinition() {
		return m_spotLightsSizeConstDefinition;
	}

	std::string& ShaderPropertiesComponent::activeDirectionalLightsConstDefinition() {
		return m_activeDirectionalLightsConstDefinition;
	}
	std::string& ShaderPropertiesComponent::activePointLightsConstDefinition() {
		return m_activePointLightsConstDefinition;
	}
	std::string& ShaderPropertiesComponent::activeSpotLightsConstDefinition() {
		return m_activeSpotLightsConstDefinition;
	}
	
}