#include <glad/glad.h>
#include "../../CForge/Graphics/RenderDevice.h"
#include "PPScreenQuad.h"

using namespace Eigen;

namespace CForge {

	void PPScreenQuad::render(RenderDevice* pRDev, float mapHeight, float uTime, float aspectRatio) {
		m_VertexArray.bind();

		if (nullptr != m_pShader) pRDev->activeShader(m_pShader);
		glUniform1f(m_pShader->uniformLocation("mapHeight"), mapHeight);
		glUniform1f(m_pShader->uniformLocation("uTime"), uTime);
		glUniform1f(m_pShader->uniformLocation("aspectRatio"), aspectRatio);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}//render

}//name space