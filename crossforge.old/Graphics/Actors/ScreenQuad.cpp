#include "../OpenGLHeader.h"
#include "../RenderDevice.h"
#include "ScreenQuad.h"

using namespace Eigen;

namespace CForge {

	ScreenQuad::ScreenQuad(void): IRenderableActor("ScreenQuad", ATYPE_SCREENQUAD) {
		m_pShader = nullptr;

	}//Constructor

	ScreenQuad::~ScreenQuad(void) {
		clear();
	}//Destructor

	void ScreenQuad::init(float left, float top, float right, float bottom, GLShader *pShader) {
		clear();

		// mapping from [0,1] -> [-1,1] (NDC)
		left = left * 2.0f - 1.0f;
		top = top * 2.0f - 1.0f;
		right = right * 2.0f - 1.0f;
		bottom = bottom * 2.0f - 1.0f;

		top *= -1.0f;
		bottom *= -1.0f;

		float QuadVertices[] = {
			left, bottom,		0.0f, 0.0f,
			right, top,			1.0f, 1.0f,
			left, top,			0.0f, 1.0f,
			
			left, bottom,		0.0f, 0.0f,
			right, bottom,		1.0f, 0.0f,
			right, top,			1.0f, 1.0f		
		};

		m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, QuadVertices, sizeof(QuadVertices));

		m_VertexArray.init();
		m_VertexArray.bind();
		setBufferData();
		m_VertexArray.unbind();
		
		m_pShader = pShader;
	}//initialize

	void ScreenQuad::setBufferData(void) {
		const uint32_t VertexSize = 4 * sizeof(float);
		const uint32_t PositionOffset = 0;
		const uint32_t UVOffset = 2 * sizeof(float);

		m_VertexBuffer.bind();
		// enable vertex and UVS
		glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
		glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION), 2, GL_FLOAT, GL_FALSE, VertexSize, (const void*)uint64_t(PositionOffset));

		glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW));
		glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW), 2, GL_FLOAT, GL_FALSE, VertexSize, (const void*)uint64_t(UVOffset));
	}//setBufferData

	void ScreenQuad::clear(void) {
		m_VertexArray.clear();
		m_VertexBuffer.clear();
		m_ElementBuffer.clear();
		m_pShader = nullptr;
	}//clear


	void ScreenQuad::render(RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) {
		if (nullptr != m_pShader) pRDev->activeShader(m_pShader);
		m_VertexArray.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		m_VertexArray.unbind();
	}//render

}//name space