#include <glad/glad.h>
#include "VertexBufferComponent.h"

namespace crossforge {

	VertexBufferComponent::VertexBufferComponent(): ComponentBase(VertexBufferComponent::identification) {
		m_glBufferHandle = GL_INVALID_INDEX;
		m_attributeMask = 0;
		GeneralUtility::memset<uint32_t>(m_attributeOffsets, 0, 8);
		m_vertexSize = 0;
		m_vertexCount = 0;
		initialize();
	}
	VertexBufferComponent::VertexBufferComponent(const std::string childIdentification): ComponentBase(VertexBufferComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}
	VertexBufferComponent::~VertexBufferComponent() {
		clear();
	}

	void VertexBufferComponent::initialize() {
		clear();
	}
	void VertexBufferComponent::clear() {
		if (glIsBuffer(m_glBufferHandle)) {
			LogWarning("OpenGL buffer with index " + std::to_string(m_glBufferHandle) + " was no deleted. Trying to clean that up for you.");
			glDeleteBuffers(1, &m_glBufferHandle);
		}
		m_glBufferHandle = GL_INVALID_INDEX;
		m_attributeMask = 0;
		GeneralUtility::memset<uint32_t>(m_attributeOffsets, 0, 8);
		m_vertexSize = 0;
		m_vertexCount = 0;
	}

	bool VertexBufferComponent::hasAttribute(VertexAttribute attrib)const {
		return ((m_attributeMask & attrib) != 0);
	}
	uint16_t& VertexBufferComponent::attributeMask() {
		return m_attributeMask;
	}
	uint32_t& VertexBufferComponent::glBufferHandle() {
		return m_glBufferHandle;
	}
	uint32_t& VertexBufferComponent::attributeOffset(VertexAttribute attrib) {
		switch (attrib) {
		case ATTRIBUTE_POSITION:	return m_attributeOffsets[0]; break;
		case ATTRIBUTE_NORMAL:		return m_attributeOffsets[1];  break;
		case ATTRIBUTE_TANGENT:		return m_attributeOffsets[2]; break;
		case ATTRIBUTE_UVW:			return m_attributeOffsets[3]; break;
		case ATTRIBUTE_COLOR:		return m_attributeOffsets[4]; break;
		case ATTRIBUTE_BONE_WEIGHTS: return m_attributeOffsets[5]; break;
		case ATTRIBUTE_BONE_INDICES:return m_attributeOffsets[6]; break;
		default: {
			throw IndexOutOfBoundsExcept("attrib");
		}break;
		}
		return m_attributeOffsets[7];
	}
	uint32_t& VertexBufferComponent::vertexSize() {
		return m_vertexSize;
	}
	uint32_t& VertexBufferComponent::vertexCount() {
		return m_vertexCount;
	}

}
