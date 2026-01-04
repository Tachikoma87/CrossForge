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

	void ShaderPropertiesComponent::initialize(const std::shared_ptr<const ShaderPropertiesComponent> pRef) {
		if (this != pRef.get()) {
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

			m_directionalLightsSizeConstDefinition = "const uint DirLightSize";
			m_pointLightsSizeConstDefinition = "const uint PointLightSize";
			m_spotLightsSizeConstDefinition = "const uint SpotLightSize";
			m_activeDirectionalLightsConstDefinition = "const uint ActiveDirLights";
			m_activePointLightsConstDefinition = "const uint ActivePointights";
			m_activeSpotLightsConstDefinition = "const uint ActiveSpotLights";
		}

		if (nullptr != pRef) {
			m_directionalLightsSize = pRef->getDirectionalLightsSize();
			m_pointLightsSize = pRef->getPointLightsSize();
			m_spotLightsSize = pRef->getSpotLightsSize();
			m_shadowMapCount = pRef->getShadowMapCount();

			m_activeDirectionalLightsCount = pRef->getActiveDirectionalLightsCount();
			m_activePointLightsCount = pRef->getActivePointLightsCount();
			m_activeSpotLightsCount = pRef->getActiveSpotLightsCount();
			m_featureMask = pRef->getFeatureMask();

			m_directionalLightsSizeConstDefinition = pRef->getDirectionalLightsSizeConstDefinition();
			m_pointLightsSizeConstDefinition = pRef->getPointLightsSizeConstDefinition();
			m_spotLightsSizeConstDefinition = pRef->getSpotLightsSizeConstDefinition();
			m_activeDirectionalLightsConstDefinition = pRef->getActiveDirectionalLightsConstDefinition();
			m_activePointLightsConstDefinition = pRef->getActivePointLightsConstDefinition();
			m_activeSpotLightsConstDefinition = pRef->getActiveSpotLightsConstDefinition();
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

	std::string& ShaderPropertiesComponent::featureName(ShaderFeature feature) {
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
	

	/** Getter **/
	const uint32_t ShaderPropertiesComponent::getDirectionalLightsSize()const {
		return m_directionalLightsSize;
	}
	const uint32_t ShaderPropertiesComponent::getPointLightsSize()const {
		return m_pointLightsSize;
	}
	const uint32_t ShaderPropertiesComponent::getSpotLightsSize()const {
		return m_spotLightsSize;
	}
	const uint32_t ShaderPropertiesComponent::getActiveDirectionalLightsCount()const {
		return m_activeDirectionalLightsCount;
	}
	const uint32_t ShaderPropertiesComponent::getActivePointLightsCount()const {
		return m_activePointLightsCount;
	}
	const uint32_t ShaderPropertiesComponent::getActiveSpotLightsCount()const {
		return m_activeSpotLightsCount;
	}
	const uint32_t ShaderPropertiesComponent::getShadowMapCount()const {
		return m_shadowMapCount;
	}

	const std::string ShaderPropertiesComponent::getFeatureName(const ShaderFeature feature)const {
		const auto element = this->m_featureNamesMap.find(feature);
		if (m_featureNamesMap.end() == element) throw IndexOutOfBoundsExcept("feature");
		return element->second;
	}
	const std::string ShaderPropertiesComponent::getDirectionalLightsSizeConstDefinition()const {
		return m_directionalLightsSizeConstDefinition;
	}
	const std::string ShaderPropertiesComponent::getPointLightsSizeConstDefinition()const {
		return m_pointLightsSizeConstDefinition;
	}
	const std::string ShaderPropertiesComponent::getSpotLightsSizeConstDefinition()const {
		return m_spotLightsSizeConstDefinition;
	}
	const std::string ShaderPropertiesComponent::getActiveDirectionalLightsConstDefinition()const {
		return m_activeDirectionalLightsConstDefinition;
	}
	const std::string ShaderPropertiesComponent::getActivePointLightsConstDefinition()const {
		return m_activePointLightsConstDefinition;
	}
	const std::string ShaderPropertiesComponent::getActiveSpotLightsConstDefinition()const {
		return m_activeSpotLightsConstDefinition;
	}

	const uint16_t ShaderPropertiesComponent::getFeatureMask()const {
		return m_featureMask;
	}

	/** Setter **/
	void ShaderPropertiesComponent::setDirectionalLightsSize(const uint32_t size) {
		m_directionalLightsSize = size;
	}
	void ShaderPropertiesComponent::setPointLightsSize(const uint32_t size) {
		m_pointLightsSize = size;
	}
	void ShaderPropertiesComponent::setSpotLightsSize(const uint32_t size) {
		m_spotLightsSize = size;
	}
	void ShaderPropertiesComponent::setActiveDirectionalLightsCount(const uint32_t count) {
		m_activeDirectionalLightsCount = count;
	}
	void ShaderPropertiesComponent::setActivePointLightsCount(const uint32_t count) {
		m_activePointLightsCount = count;
	}
	void ShaderPropertiesComponent::setActiveSpotLightsCount(const uint32_t count) {
		m_activeSpotLightsCount = count;
	}
	void ShaderPropertiesComponent::setShadowMapCount(const uint32_t count) {
		m_shadowMapCount = count;
	}

	void ShaderPropertiesComponent::setFeatureName(const std::string name, const ShaderFeature feature) {
		auto element = m_featureNamesMap.find(feature);
		if (m_featureNamesMap.end() == element) throw IndexOutOfBoundsExcept("feature");
		m_featureNamesMap[feature] = name;
	}
	void ShaderPropertiesComponent::setDirectionalLightsSizeConstDefinition(const std::string definition) {
		m_directionalLightsSizeConstDefinition = definition;
	}
	void ShaderPropertiesComponent::setPointLightsSizeConstDefinition(const std::string definition) {
		m_pointLightsSizeConstDefinition = definition;
	}
	void ShaderPropertiesComponent::setSpotLightsSizeConstDefinition(const std::string definition) {
		m_spotLightsSizeConstDefinition = definition;
	}
	void ShaderPropertiesComponent::setActiveDirectionalLightsConstDefinition(const std::string definition) {
		m_activeDirectionalLightsConstDefinition = definition;
	}
	void ShaderPropertiesComponent::setActivePointLightsConstDefinition(const std::string definition) {
		m_activePointLightsConstDefinition = definition;
	}
	void ShaderPropertiesComponent::setActiveSpotLightsConstDefinition(const std::string definition) {
		m_activeSpotLightsConstDefinition = definition;
	}

	void ShaderPropertiesComponent::setFeatureMask(uint16_t featureMask) {
		m_featureMask = featureMask;
	}

}