#include "LightsEntity.h"

namespace crossforge {

	LightsEntity::LightsEntity(uint8_t componentsMap): EntityBase(LightsEntity::identification) {
		initialize(componentsMap);
	}
	LightsEntity::LightsEntity(const std::string childIdentification): EntityBase(LightsEntity::identification) {
		m_inheritance.push_back(childIdentification);

	}
	LightsEntity::~LightsEntity() {
		clear();
	}

	void LightsEntity::initialize(uint8_t componentsMask) {
		clear();
		if (componentsMask & COMPONENT_LIGHTS_CONFIG) addComponent(std::make_shared<LightsConfigComponent>());
		if (componentsMask & COMPONENT_DIRECTIONAL_LIGHTS_UBO) addComponent(std::make_shared<UBODirectionalLightsComponent>());
		if (componentsMask & COMPONENT_POINT_LIGHTS_UBO) addComponent(std::make_shared<UBOPointLightsComponent>());
		if (componentsMask & COMPONENT_SPOT_LIGHTS_UBO) addComponent(std::make_shared<UBOSpotLightsComponent>());

	}
	void LightsEntity::clear() {
		m_componentMap.clear();
	}

	SceneLightsConfigComponentPtr LightsEntity::getLightsConfigComponent() {
		return getComponent<LightsConfigComponent>();
	}
	UBODirectionalLightsComponentPtr LightsEntity::getUBODirectionalLightsComponent() {
		return getComponent<UBODirectionalLightsComponent>();
	}
	UBOPointLightsComponentPtr LightsEntity::getUBOPointLightsComponent() {
		return getComponent<UBOPointLightsComponent>();
	}
	UBOSpotLightsComponentPtr LightsEntity::getUBOSpotLightsComponent() {
		return getComponent<UBOSpotLightsComponent>();
	}
}