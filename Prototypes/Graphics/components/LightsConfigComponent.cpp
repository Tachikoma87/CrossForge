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

	void LightsConfigComponent::initialize(const std::shared_ptr<const LightsConfigComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_activeDirectionalLights = pRef->getActiveDirectionalLights();
			m_activePointLights = pRef->getActivePointLights();
			m_activeSpotLights = pRef->getActiveSpotLights();
			m_directionalLightsSize = pRef->getDirectionalLightsSize();
			m_pointLightsSize = pRef->getPointLightsSize();
			m_spotLightsSize = pRef->getSpotLightsSize();
		}
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

	uint32_t& LightsConfigComponent::directionalLightsSize() {
		return m_directionalLightsSize;
	}
	uint32_t& LightsConfigComponent::pointLightsSize() {
		return m_pointLightsSize;
	}
	uint32_t& LightsConfigComponent::spotLightsSize() {
		return m_spotLightsSize;
	}

	const uint32_t LightsConfigComponent::getActiveDirectionalLights()const {
		return m_activeDirectionalLights;
	}
	const uint32_t LightsConfigComponent::getActivePointLights()const {
		return m_activePointLights;
	}
	const uint32_t LightsConfigComponent::getActiveSpotLights()const {
		return m_activeSpotLights;
	}
	const uint32_t LightsConfigComponent::getDirectionalLightsSize()const {
		return m_directionalLightsSize;
	}
	const uint32_t LightsConfigComponent::getPointLightsSize()const {
		return m_pointLightsSize;
	}
	const uint32_t LightsConfigComponent::getSpotLightsSize()const {
		return m_spotLightsSize;
	}

	/** Setter **/
	void LightsConfigComponent::setActiveDirectionalLights(const uint32_t activeLights) {
		m_activeDirectionalLights = activeLights;
	}
	void LightsConfigComponent::setActivePointLights(const uint32_t activeLights) {
		m_activePointLights = activeLights;
	}
	void LightsConfigComponent::setActiveSpotLights(const uint32_t activeLights) {
		m_activeSpotLights = activeLights;
	}
	void LightsConfigComponent::setDirectionalLightsSize(const uint32_t size) {
		m_directionalLightsSize = size;
	}
	void LightsConfigComponent::setPointLightsSize(const uint32_t size) {
		m_pointLightsSize = size;
	}
	void LightsConfigComponent::setSpotLightsSize(const uint32_t size) {
		m_spotLightsSize = size;
	}

}