#include "LightEntity.h"

namespace crossforge {

	LightEntity::LightEntity(uint8_t componentsBitmask): SceneObjectEntity(LightEntity::identification) {
		initialize(componentsBitmask);
	}
	LightEntity::LightEntity(const std::string childIdentification): SceneObjectEntity(LightEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}
	LightEntity::~LightEntity() {
		clear();
	}

	void LightEntity::initialize(uint8_t componentsBitmask) {
		clear();
	}
	void LightEntity::clear() {
		m_componentMap.clear();
	}

}