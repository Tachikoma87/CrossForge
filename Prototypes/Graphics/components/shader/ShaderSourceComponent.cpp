#include "ShaderSourceComponent.h"

namespace crossforge {

	ShaderSourceComponent::ShaderSourceComponent(): ComponentBase(ShaderSourceComponent::identification) {
		
	}
	ShaderSourceComponent::~ShaderSourceComponent() {
		clear();
	}
	ShaderSourceComponent::ShaderSourceComponent(const std::string childIdentification): ComponentBase(ShaderSourceComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void ShaderSourceComponent::initialize() {
		clear();
	}
	void ShaderSourceComponent::clear() {
		m_vertexShaderSources.clear();
		m_fragmentShaderSources.clear();
		m_computeShaderSources.clear();
	}

	std::vector<std::string>& ShaderSourceComponent::vertexShaderSources() {
		return m_vertexShaderSources;
	}
	std::vector<std::string>& ShaderSourceComponent::fragmentShaderSources() {
		return m_fragmentShaderSources;
	}
	std::vector<std::string>& ShaderSourceComponent::computeShaderSources() {
		return m_computeShaderSources;
	}
}