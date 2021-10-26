#include <glad/glad.h>
#include "IRenderableActor.h"
#include "../RenderDevice.h"

namespace CForge {

	IRenderableActor::IRenderableActor(const std::string ClassName, int32_t ActorType): CForgeObject("IRenderableActor::" + ClassName) {
		m_TypeID = ActorType;
	}//Constructor

	IRenderableActor::~IRenderableActor(void) {

	}//Destructor

	int32_t IRenderableActor::typeID(void)const {
		return m_TypeID;
	}//typeID


	void IRenderableActor::setBufferData(void) {

		// bind array and index buffer
		m_VertexBuffer.bind();
		m_ElementBuffer.bind();

		// position array
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_POSITION)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)(m_VertexUtility.offset(VertexUtility::VPROP_POSITION)));
		}

		// normal array
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_NORMAL)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_NORMAL));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_NORMAL), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)m_VertexUtility.offset(VertexUtility::VPROP_NORMAL));
		}

		// tangent array
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_TANGENT)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_TANGENT));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_TANGENT), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)m_VertexUtility.offset(VertexUtility::VPROP_TANGENT));
		}

		// texture coordinates
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_UVW)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)m_VertexUtility.offset(VertexUtility::VPROP_UVW));
		}

		// vertex colors
		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_COLOR)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_COLOR));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_COLOR), 3, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (void*)m_VertexUtility.offset(VertexUtility::VPROP_COLOR));
		}

		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_BONEINDICES)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_BONE_INDICES));
			glVertexAttribIPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_BONE_INDICES), 4, GL_INT, m_VertexUtility.vertexSize(), (const void*)m_VertexUtility.offset(VertexUtility::VPROP_BONEINDICES));
		}

		if (m_VertexUtility.hasProperties(VertexUtility::VPROP_BONEWEIGHTS)) {
			glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_BONE_WEIGHTS));
			glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_BONE_WEIGHTS), 4, GL_FLOAT, GL_FALSE, m_VertexUtility.vertexSize(), (const void*)m_VertexUtility.offset(VertexUtility::VPROP_BONEWEIGHTS));
		}


	}//setBufferData

}//name space