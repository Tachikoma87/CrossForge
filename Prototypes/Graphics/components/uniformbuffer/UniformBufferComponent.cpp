#include <glad/glad.h>
#include "UniformBufferComponent.h"
#include "../../../utility/GraphicsUtility.h"

namespace crossforge {
	uint32_t UniformBufferComponent::getGlBufferHandle() {
		return m_glBufferHandle;
	}
	uint32_t UniformBufferComponent::getBufferSize() {
		return m_bufferSize;
	}

	UniformBufferComponent::UniformBufferComponent(const std::string childIdentification) : ComponentBase(UniformBufferComponent::identification) {
		m_inheritance.push_back(childIdentification);
		m_glBufferHandle = GL_INVALID_INDEX;
		m_bufferSize = 0;
	}

	UniformBufferComponent::~UniformBufferComponent() {
		clear();
	}
	

	bool UniformBufferComponent::initialize(uint32_t bufferSize, bool clearData) {
		if(clearData) clear();
		if (!glIsBuffer(m_glBufferHandle)) glGenBuffers(1, &m_glBufferHandle);
		glBindBuffer(GL_UNIFORM_BUFFER, m_glBufferHandle);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		std::string errorLog;
		uint32_t error = GraphicsUtility::checkGLError(&errorLog);
		if (GL_NO_ERROR != error) LogError("OpenGL error occurred during uniform buffer initialization: " + errorLog);
		else m_bufferSize = bufferSize;
		return (GL_NO_ERROR == error);
	}

	void UniformBufferComponent::clear() {
		if (glIsBuffer(m_glBufferHandle)) glDeleteBuffers(1, &m_glBufferHandle);
		m_glBufferHandle = GL_INVALID_INDEX;
		m_bufferSize = 0;
	}

	void UniformBufferComponent::setSubData(const void* pData, uint32_t dataSize, uint32_t bufferOffset) {
		if (0 == m_bufferSize) throw NotInitializedExcept("Uniform buffer");
		glBindBuffer(GL_UNIFORM_BUFFER, m_glBufferHandle);
		glBufferSubData(GL_UNIFORM_BUFFER, bufferOffset, dataSize, pData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}