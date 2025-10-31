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
		if (componentsBitmask & SHADER_SOURCE_COMPONENT) addComponent(std::make_shared<ShaderSourceComponent>());
		if (componentsBitmask & RENDERING_SHADER_COMPONENT) addComponent(std::make_shared<RenderingShaderComponent>());
		if (componentsBitmask & SHADER_PROPERTIES_COMPONENT) addComponent(std::make_shared<ShaderPropertiesComponent>());
	}
	void ShaderEntity::clear() {
		m_componentMap.clear();
	}

	ShaderSourceComponentPtr  ShaderEntity::getShaderSourceComponent() {
		return getComponent<ShaderSourceComponent>();
	}
	RenderingShaderComponentPtr  ShaderEntity::getRenderingShaderComponent() {
		return getComponent<RenderingShaderComponent>();
	}
	ShaderPropertiesComponentPtr ShaderEntity::getShaderPropertiesComponent() {
		return getComponent<ShaderPropertiesComponent>();
	}
}