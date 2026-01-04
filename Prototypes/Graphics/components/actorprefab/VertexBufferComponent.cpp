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

	void VertexBufferComponent::initialize(const std::shared_ptr<const VertexBufferComponent> pRef) {
		if (this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_glBufferHandle = pRef->getGlBufferHandle();
			m_attributeMask = pRef->getAttributeMask();;
			m_attributeOffsets[0] = pRef->getAttributeOffset(ATTRIBUTE_POSITION);
			m_attributeOffsets[1] = pRef->getAttributeOffset(ATTRIBUTE_NORMAL);
			m_attributeOffsets[2] = pRef->getAttributeOffset(ATTRIBUTE_TANGENT);
			m_attributeOffsets[3] = pRef->getAttributeOffset(ATTRIBUTE_UVW);
			m_attributeOffsets[4] = pRef->getAttributeOffset(ATTRIBUTE_COLOR);
			m_attributeOffsets[5] = pRef->getAttributeOffset(ATTRIBUTE_BONE_WEIGHTS);
			m_attributeOffsets[6] = pRef->getAttributeOffset(ATTRIBUTE_BONE_INDICES);
			m_vertexSize = pRef->getVertexSize();
			m_vertexCount = pRef->getVertexCount();
		}
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

	const bool VertexBufferComponent::hasAttribute(VertexAttribute attrib)const {
		return ((m_attributeMask & attrib) != 0);
	}
	void VertexBufferComponent::addAttribute(VertexAttribute attrib) {
		m_attributeMask |= attrib;
	}
	void VertexBufferComponent::removeAttribute(VertexAttribute attrib) {
		m_attributeMask = m_attributeMask ^ (m_attributeMask & attrib);
	}

	uint16_t& VertexBufferComponent::attributeMask() {
		return m_attributeMask;
	}
	uint32_t& VertexBufferComponent::glBufferHandle() {
		return m_glBufferHandle;
	}
	uint32_t& VertexBufferComponent::attributeOffset(VertexAttribute attrib) {
		const auto index = getIndexFromAttribute(attrib);
		if (0 > index || index >= 8) throw IndexOutOfBoundsExcept("attrib");
		return m_attributeOffsets[index];
	}
	uint32_t& VertexBufferComponent::vertexSize() {
		return m_vertexSize;
	}
	uint32_t& VertexBufferComponent::vertexCount() {
		return m_vertexCount;
	}


	const uint16_t VertexBufferComponent::getAttributeMask()const {
		return m_attributeMask;
	}
	const uint32_t VertexBufferComponent::getGlBufferHandle()const {
		return m_glBufferHandle;
	}
	const uint32_t VertexBufferComponent::getAttributeOffset(const VertexAttribute attrib)const {
		const auto index = getIndexFromAttribute(attrib);
		if (0 > index || index >= 8) throw IndexOutOfBoundsExcept("attrib");
		return m_attributeOffsets[index];
		
	}
	const uint32_t VertexBufferComponent::getVertexSize()const {
		return m_vertexSize;
	}
	const uint32_t VertexBufferComponent::getVertexCount()const {
		return m_vertexCount;
	}

	void VertexBufferComponent::setAttributeMask(const uint16_t attributeMask) {
		m_attributeMask = attributeMask;
	}
	void VertexBufferComponent::setGlBufferHandle(const uint32_t handle) {
		m_glBufferHandle = handle;
	}
	void VertexBufferComponent::setAttributeOffset(const uint32_t offset, VertexAttribute attrib) {
		const int8_t index = getIndexFromAttribute(attrib);
		if (0 > index || index >= 7) throw IndexOutOfBoundsExcept("attrib");
		m_attributeOffsets[index] = offset;
	}
	void VertexBufferComponent::setVertexSize(const uint32_t vertexSize) {
		m_vertexSize = vertexSize;
	}
	void VertexBufferComponent::setVertexCount(const uint32_t vertexCount) {
		m_vertexCount = vertexCount;
	}

	const int8_t VertexBufferComponent::getIndexFromAttribute(VertexAttribute attrib)const {
		int8_t result = -1;
		switch (attrib) {
		case ATTRIBUTE_POSITION:	result = 0; break;
		case ATTRIBUTE_NORMAL:		result = 1; break;
		case ATTRIBUTE_TANGENT:		result = 2; break;
		case ATTRIBUTE_UVW:			result = 3; break;
		case ATTRIBUTE_COLOR:		result = 4; break;
		case ATTRIBUTE_BONE_WEIGHTS:result = 5; break;
		case ATTRIBUTE_BONE_INDICES:result = 6; break;
		default: {
			throw IndexOutOfBoundsExcept("attrib");
		}break;
		}
		return result;
	}

}
