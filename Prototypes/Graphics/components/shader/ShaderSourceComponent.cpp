#include "ShaderSourceComponent.h"

namespace crossforge {

	ShaderSourceComponent::ShaderSourceComponent(): ComponentBase(ShaderSourceComponent::identification) {
		initialize();
	}
	ShaderSourceComponent::~ShaderSourceComponent() {
		clear();
	}
	ShaderSourceComponent::ShaderSourceComponent(const std::string childIdentification): ComponentBase(ShaderSourceComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void ShaderSourceComponent::initialize(const std::shared_ptr<const ShaderSourceComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_vertexShaderSources = pRef->getVertexShaderSources();
			m_fragmentShaderSources = pRef->getFragmentShaderSources();
			m_computeShaderSources = pRef->getComputeShaderSources();
		}
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

	const std::vector<std::string> ShaderSourceComponent::getVertexShaderSources()const {
		return m_vertexShaderSources;
	}
	const std::vector<std::string> ShaderSourceComponent::getFragmentShaderSources()const {
		return m_fragmentShaderSources;
	}
	const std::vector<std::string> ShaderSourceComponent::getComputeShaderSources()const {
		return m_computeShaderSources;
	}

	void ShaderSourceComponent::setVertexShaderSources(const std::vector<std::string> sources) {
		m_vertexShaderSources = sources;
	}
	void ShaderSourceComponent::setFragmentShaderSources(const std::vector<std::string> sources) {
		m_fragmentShaderSources = sources;
	}
	void ShaderSourceComponent::setComputeShaderSources(const std::vector<std::string> sources) {
		m_computeShaderSources = sources;
	}
}