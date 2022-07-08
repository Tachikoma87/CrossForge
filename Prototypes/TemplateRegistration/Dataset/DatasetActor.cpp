#include "DatasetActor.h"

#include <glad/glad.h>

namespace TempReg {

	DatasetActor::DatasetActor(void) : CForge::IRenderableActor("DatasetActor", 10) {

	}//Constructor

	DatasetActor::~DatasetActor(void) {
		m_Actor.clear();
	}//Destructor

	void DatasetActor::init(const CForge::T3DMesh<float>* pMesh, DatasetRenderMode RM) {
		m_Actor.init(pMesh);
		m_RenderMode = RM;
	}//initialize

	void DatasetActor::release(void) {
		delete this;
	}//release

	void DatasetActor::render(CForge::RenderDevice* pRDev) {
		switch (m_RenderMode) {
		case DatasetRenderMode::FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		case DatasetRenderMode::LINE: {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//glLineWidth(2);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1, -1);
			break;
		}
		case DatasetRenderMode::POINT: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
		default: break;
		}

		m_Actor.render(pRDev);

		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(0, 0);
		//glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}//render

	void DatasetActor::renderMode(DatasetRenderMode RM) {
		m_RenderMode = RM;
	}//renderMode

	DatasetRenderMode DatasetActor::renderMode(void)const {
		return m_RenderMode;
	}//renderMode
}