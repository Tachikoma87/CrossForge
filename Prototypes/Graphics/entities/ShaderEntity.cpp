#include "ShaderEntity.h"

namespace crossforge {

	ShaderEntity::ShaderEntity(uint8_t componentsBitmask) : EntityBase(ShaderEntity::identification) {
		initialize(componentsBitmask);
	}
	ShaderEntity::~ShaderEntity() {
		clear();
	}

	ShaderEntity::ShaderEntity(const std::string childIdentification): EntityBase(ShaderEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void ShaderEntity::initialize(uint8_t componentsBitmask) {
		clear();
	}
	void ShaderEntity::clear() {
		m_componentMap.clear();
	}
}