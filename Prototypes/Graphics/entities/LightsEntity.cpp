#include "LightsEntity.h"

namespace crossforge {

	LightsEntity::LightsEntity(): EntityBase(LightsEntity::identification) {
		initialize();
	}
	LightsEntity::LightsEntity(const std::string childIdentification): EntityBase(LightsEntity::identification) {
		m_inheritance.push_back(childIdentification);

	}
	LightsEntity::~LightsEntity() {
		clear();
	}

	void LightsEntity::initialize() {
		clear();
	}
	void LightsEntity::clear() {
		m_componentMap.clear();
	}

	LightsConfigComponentPtr LightsEntity::getLightsConfigComponent(bool createIfNotExists) {
		return getComponent<LightsConfigComponent>(createIfNotExists);
	}
	UBODirectionalLightsComponentPtr LightsEntity::getUBODirectionalLightsComponent(bool createIfNotExists) {
		return getComponent<UBODirectionalLightsComponent>(createIfNotExists);
	}
	UBOPointLightsComponentPtr LightsEntity::getUBOPointLightsComponent(bool createIfNotExists) {
		return getComponent<UBOPointLightsComponent>(createIfNotExists);
	}
	UBOSpotLightsComponentPtr LightsEntity::getUBOSpotLightsComponent(bool createIfNotExists) {
		return getComponent<UBOSpotLightsComponent>(createIfNotExists);
	}
}