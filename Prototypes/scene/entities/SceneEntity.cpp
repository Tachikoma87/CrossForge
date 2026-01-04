#include "SceneEntity.h"

namespace crossforge {

	SceneEntity::SceneEntity(): EntityBase(SceneEntity::identification) {
		initialize();
	}
	SceneEntity::~SceneEntity() {
		clear();
	}
	SceneEntity::SceneEntity(const std::string childIdentification): EntityBase(SceneEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void SceneEntity::initialize() {
		clear();
		
	}
	void SceneEntity::clear() {
		m_componentMap.clear();
	}

	


}