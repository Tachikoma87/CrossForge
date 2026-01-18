#include "LightEntity.h"

namespace crossforge {

	LightEntity::LightEntity(): SceneNodeEntity(LightEntity::identification) {
		initialize();
	}
	LightEntity::LightEntity(const std::string childIdentification): SceneNodeEntity(LightEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}
	LightEntity::~LightEntity() {
		clear();
	}

	void LightEntity::initialize() {
		clear();
	}
	void LightEntity::clear() {
		m_componentMap.clear();
	}

}