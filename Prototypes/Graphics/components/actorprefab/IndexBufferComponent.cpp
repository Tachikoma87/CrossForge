#include <glad/glad.h>
#include "IndexBufferComponent.h"

namespace crossforge {

	IndexBufferComponent::IndexBufferComponent(): ComponentBase(IndexBufferComponent::identification) {
		m_glBufferHandle = GL_INVALID_INDEX;
		m_indexCount = 0;
		m_bufferSize = 0;
	}
	IndexBufferComponent::IndexBufferComponent(const std::string childIdentification): ComponentBase(IndexBufferComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}
	IndexBufferComponent::~IndexBufferComponent() {
		if (glIsBuffer(m_glBufferHandle)) {
			LogWarning("OpenGL buffer with index " + std::to_string(m_glBufferHandle) + " was no deleted. Trying to do clean that up for you.");
			glDeleteBuffers(1, &m_glBufferHandle);
			m_glBufferHandle = GL_INVALID_INDEX;
		}
	}

	uint32_t& IndexBufferComponent::glBufferHandle() {
		return m_glBufferHandle;
	}
	uint64_t& IndexBufferComponent::indexCount() {
		return m_indexCount;
	}
	uint64_t& IndexBufferComponent::bufferSize() {
		return m_bufferSize;
	}
	
}