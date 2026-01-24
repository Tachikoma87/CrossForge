#include <CForge/Graphics/OpenGLHeader.h>
#include "../../CForge/Graphics/RenderDevice.h"
#include "PPScreenQuad.h"

using namespace Eigen;

namespace CForge {

	void PPScreenQuad::render(RenderDevice* pRDev, float mapHeight, float uTime, float aspectRatio, Eigen::Vector3f sunDir, Eigen::Vector2i resolution) {
		m_VertexArray.bind();

		if (nullptr != m_pShader) pRDev->activeShader(m_pShader);
		glUniform1f(m_pShader->uniformLocation("mapHeight"), mapHeight);
		glUniform1f(m_pShader->uniformLocation("uTime"), uTime);
		glUniform1f(m_pShader->uniformLocation("aspectRatio"), aspectRatio);
		glUniform3f(m_pShader->uniformLocation("sunDir"), sunDir.x(), sunDir.y(), sunDir.z());
		glUniform2i(m_pShader->uniformLocation("uResolution"), resolution.x(), resolution.y());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}//render
	void PPSQ_SSAO::render(RenderDevice* pRDev, float uTime, Eigen::Vector2i resolution) {
		m_VertexArray.bind();
		if (nullptr != m_pShader) pRDev->activeShader(m_pShader);
		glUniform1f(m_pShader->uniformLocation("uTime"), uTime);
		glUniform2i(m_pShader->uniformLocation("uResolution"), resolution.x(), resolution.y());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}//name space