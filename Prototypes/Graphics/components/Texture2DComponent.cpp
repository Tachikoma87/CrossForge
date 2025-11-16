#include <glad/glad.h>
#include "Texture2DComponent.h"

namespace crossforge {

	Texture2DComponent::Texture2DComponent(): ComponentBase(Texture2DComponent::identification) {
		m_glTextureHandle = GL_INVALID_INDEX;
		initialize();
	}
	Texture2DComponent::Texture2DComponent(const std::string childIdentification): ComponentBase(Texture2DComponent::identification) {
		m_inheritance.push_back(childIdentification);
		m_glTextureHandle = GL_INVALID_INDEX;
		initialize();
		
	}
	Texture2DComponent::~Texture2DComponent() {
		if (glIsTexture(m_glTextureHandle)) {
			LogWarning("Texture with id " + std::to_string(m_glTextureHandle) + " was not delete yet. Trying to clean it up for you.");
			glDeleteTextures(1, &m_glTextureHandle);
		}
	}

	void Texture2DComponent::initialize() {
		clear();
	}
	void Texture2DComponent::clear() {
		if (glIsTexture(m_glTextureHandle)) {
			LogWarning("OpenGL texture with handle " + std::to_string(m_glTextureHandle) + " was not deleted properly. Will try to do this for you.");
			glDeleteTextures(1, &m_glTextureHandle);
		}
		m_glTextureHandle = GL_INVALID_INDEX;
		m_width = 0;
		m_height = 0;
	}

	uint32_t& Texture2DComponent::glTextureHandle() {
		return m_glTextureHandle;
	}
	uint32_t& Texture2DComponent::width() {
		return m_width;
	}
	uint32_t& Texture2DComponent::height() {
		return m_height;
	}
}