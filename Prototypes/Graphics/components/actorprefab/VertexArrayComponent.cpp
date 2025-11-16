#include <glad/glad.h>
#include "VertexArrayComponent.h"

namespace crossforge {

	VertexArrayComponent::VertexArrayComponent(): ComponentBase(VertexArrayComponent::identification) {
		m_glVertexArrayHandle = GL_INVALID_INDEX;
		initialize();
	}
	VertexArrayComponent::VertexArrayComponent(const std::string childIdentification) : ComponentBase(VertexArrayComponent::identification) {
		m_inheritance.push_back(childIdentification);
		m_glVertexArrayHandle = GL_INVALID_INDEX;
		initialize();
	}
	VertexArrayComponent::~VertexArrayComponent() {
		clear();
	}
	

	void VertexArrayComponent::initialize() {
		clear();

	}
	void VertexArrayComponent::clear() {
		if (glIsVertexArray(m_glVertexArrayHandle)) {
			LogWarning("OpenGL vertex array with index " + std::to_string(m_glVertexArrayHandle) + " was not deleted. Trying to clean that up for you.");
			glDeleteVertexArrays(1, &m_glVertexArrayHandle);
		}
	}

	uint32_t& VertexArrayComponent::glVertexArrayHandle() {
		return m_glVertexArrayHandle;
	}
}