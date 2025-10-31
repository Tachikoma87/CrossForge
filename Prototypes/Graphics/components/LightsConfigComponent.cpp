#include "LightsConfigComponent.h"

namespace crossforge {

	LightsConfigComponent::LightsConfigComponent(): ComponentBase(LightsConfigComponent::identification) {
		initialize();
	}
	LightsConfigComponent::~LightsConfigComponent() {

	}
	LightsConfigComponent::LightsConfigComponent(const std::string childIdentification): ComponentBase(LightsConfigComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void LightsConfigComponent::initialize() {
		clear();
	}
	void LightsConfigComponent::clear() {
		m_activeDirectionalLights = 0;
		m_activePointLights = 0;
		m_activeSpotLights = 0;
		m_directionalLightsSize = 0;
		m_pointLightsSize = 0;
		m_spotLightsSize = 0;
	}

	uint32_t& LightsConfigComponent::activeDirectionalLights() {
		return m_activeDirectionalLights;
	}
	uint32_t& LightsConfigComponent::activePointLights() {
		return m_activePointLights;
	}
	uint32_t& LightsConfigComponent::activeSpotLights() {
		return m_activeSpotLights;
	}

	uint32_t& LightsConfigComponent::directionalLightsUBOSize() {
		return m_directionalLightsSize;
	}
	uint32_t& LightsConfigComponent::pointLightsUBOSize() {
		return m_pointLightsSize;
	}
	uint32_t& LightsConfigComponent::spotLightsUBSSize() {
		return m_spotLightsSize;
	}

}