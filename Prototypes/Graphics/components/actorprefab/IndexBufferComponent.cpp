#include <glad/glad.h>
#include "IndexBufferComponent.h"

namespace crossforge {

	IndexBufferComponent::IndexBufferComponent(): ComponentBase(IndexBufferComponent::identification) {
		m_glBufferHandle = GL_INVALID_INDEX;
		initialize();
	}
	IndexBufferComponent::IndexBufferComponent(const std::string childIdentification): ComponentBase(IndexBufferComponent::identification) {
		m_inheritance.push_back(childIdentification);
		m_glBufferHandle = GL_INVALID_INDEX;
		initialize();
	}
	IndexBufferComponent::~IndexBufferComponent() {
		clear();
	}

	void IndexBufferComponent::initialize(const std::shared_ptr<const IndexBufferComponent> pRef) {
		if (this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_glBufferHandle = pRef->getGlBufferHandle();
			m_indexCount = pRef->getIndexCount();
			m_bufferSize = pRef->getBufferSize();
		}
	}
	void IndexBufferComponent::clear() {
		if (glIsBuffer(m_glBufferHandle)) {
			LogWarning("OpenGL buffer with index " + std::to_string(m_glBufferHandle) + " was no deleted. Trying to do clean that up for you.");
			glDeleteBuffers(1, &m_glBufferHandle);
		}
		m_glBufferHandle = GL_INVALID_INDEX;
		m_indexCount = 0;
		m_bufferSize = 0;
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
	
	const uint32_t IndexBufferComponent::getGlBufferHandle()const {
		return m_glBufferHandle;
	}
	const uint64_t IndexBufferComponent::getIndexCount()const {
		return m_indexCount;
	}
	const uint64_t IndexBufferComponent::getBufferSize()const {
		return m_bufferSize;
	}

	void IndexBufferComponent::setGlBufferHandle(const uint32_t handle) {
		m_glBufferHandle = handle;
	}
	void IndexBufferComponent::setIndexCount(const uint64_t indexCount) {
		m_indexCount = indexCount;
	}
	void IndexBufferComponent::setBufferSize(const uint64_t bufferSize) {
		m_bufferSize = bufferSize;
	}
}