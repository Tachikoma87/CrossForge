#include "ShaderEntity.h"

namespace crossforge {

	ShaderEntity::ShaderEntity() : EntityBase(ShaderEntity::identification) {
		initialize();
	}
	ShaderEntity::~ShaderEntity() {
		clear();
	}

	ShaderEntity::ShaderEntity(const std::string childIdentification): EntityBase(ShaderEntity::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void ShaderEntity::initialize() {
		clear();
	}
	void ShaderEntity::clear() {
		m_componentMap.clear();
	}

	ShaderSourceComponentPtr  ShaderEntity::getShaderSourceComponent(bool createIfNotExists) {
		return getComponent<ShaderSourceComponent>(createIfNotExists);
	}
	RenderingShaderComponentPtr  ShaderEntity::getRenderingShaderComponent(bool createIfNotExists) {
		return getComponent<RenderingShaderComponent>(createIfNotExists);
	}
	ShaderPropertiesComponentPtr ShaderEntity::getShaderPropertiesComponent(bool createIfNotExists) {
		return getComponent<ShaderPropertiesComponent>(createIfNotExists);
	}
}