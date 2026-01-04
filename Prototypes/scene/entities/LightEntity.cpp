#include "LightEntity.h"

namespace crossforge {

	LightEntity::LightEntity(): SceneObjectEntity(LightEntity::identification) {
		initialize();
	}
	LightEntity::LightEntity(const std::string childIdentification): SceneObjectEntity(LightEntity::identification) {
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