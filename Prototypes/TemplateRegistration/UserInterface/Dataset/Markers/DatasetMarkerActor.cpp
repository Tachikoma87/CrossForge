#include "DatasetMarkerActor.h"

#include <glad/glad.h>

namespace TempReg {

	DatasetMarkerActor::DatasetMarkerActor(void) : CForge::IRenderableActor("DatasetActor", 11) {
		m_UsePolygonOffset = false;
	}//Constructor

	DatasetMarkerActor::~DatasetMarkerActor() {
		m_Actor.clear();
	}//Destructor

	void DatasetMarkerActor::init(const CForge::T3DMesh<float>* pMesh, bool UseOffset) {
		m_Actor.init(pMesh);
		m_UsePolygonOffset = UseOffset;
	}//init

	void DatasetMarkerActor::offset(bool UseOffset) {
		m_UsePolygonOffset = UseOffset;
	}//offset

	void DatasetMarkerActor::release(void) {
		delete this;
	}//release

	void DatasetMarkerActor::render(CForge::RenderDevice* pRDev) {
		if (m_UsePolygonOffset) {
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(-1, -1);
			m_Actor.render(pRDev);
			glPolygonOffset(0, 0);
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
		else {
			m_Actor.render(pRDev);
		}
	}//render
}