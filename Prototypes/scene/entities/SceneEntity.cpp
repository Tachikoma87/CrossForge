#include "SceneEntity.h"

namespace crossforge {

	SceneEntity::SceneEntity(uint8_t componentsMask): EntityBase(SceneEntity::identification) {
		initialize(componentsMask);
	}
	SceneEntity::~SceneEntity() {
		clear();
	}
	SceneEntity::SceneEntity(const std::string childIdentification): EntityBase(SceneEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void SceneEntity::initialize(uint8_t componentsMask) {
		clear();
		
	}
	void SceneEntity::clear() {
		m_componentMap.clear();
	}

	


}