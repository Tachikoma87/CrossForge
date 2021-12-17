#include <glad/glad.h>
#include "InstanceActor.h"
#include "../RenderDevice.h"
#include "../GraphicsUtility.h"
#include "../../Core/SLogger.h"

namespace CForge {
	InstanceActor::InstanceActor(void): IRenderableActor("InstanceActor", ATYPE_STATIC) {
		m_TypeID = ATYPE_STATIC;
		m_TypeName = "Instance Actor";
	}//Constructor

	InstanceActor::~InstanceActor(void) {
		clear();
	}//Destructor

	void InstanceActor::init(const T3DMesh<float>* pMesh) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data");

		uint16_t VertexProperties = 0;
		if (pMesh->vertexCount() > 0) VertexProperties |= VertexUtility::VPROP_POSITION;
		if (pMesh->normalCount() > 0) VertexProperties |= VertexUtility::VPROP_NORMAL;
		if (pMesh->tangentCount() > 0) VertexProperties |= VertexUtility::VPROP_TANGENT;
		if (pMesh->textureCoordinatesCount() > 0) VertexProperties |= VertexUtility::VPROP_UVW;
		if (pMesh->colorCount() > 0) VertexProperties |= VertexUtility::VPROP_COLOR;
		
		// build array buffer of vertex data
		void* pBuffer = nullptr;
		uint32_t BufferSize = 0;

		try {
			m_VertexUtility.init(VertexProperties);
			m_VertexUtility.buildBuffer(pMesh->vertexCount(), &pBuffer, &BufferSize, pMesh);
			m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
			// free buffer data
			delete[] pBuffer;
			pBuffer = nullptr;
			BufferSize = 0;
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
			return;
		}
		catch (...) {
			SLogger::log("Unknown exception occurred during vertex buffer creation!");
			return;
		}
	
		// build render groups and element array
		try {
			m_RenderGroupUtility.init(pMesh, &pBuffer, &BufferSize);
			m_ElementBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
			// free buffer data
			delete[] pBuffer;
			pBuffer = nullptr;
			BufferSize = 0;
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
			return;
		}
		catch (...) {
			SLogger::log("Unknown exception occurred during building of index buffer!");
			return;
		}
		
		m_VertexArray.init();
		m_VertexArray.bind();
		setBufferData();
		m_VertexArray.unbind();
			
	}//initialize

	void InstanceActor::clear(void) {
		m_VertexBuffer.clear();
		m_ElementBuffer.clear(); 
		m_VertexArray.clear();

		m_VertexUtility.clear();
		m_RenderGroupUtility.clear();
	}//Clear

	void InstanceActor::release(void) {
		delete this;
	}//release

	uint32_t InstanceActor::materialCount(void) const {
		return m_RenderGroupUtility.renderGroupCount();
	}//materialCount

	RenderMaterial* InstanceActor::material(uint32_t Index) {
		if (Index >= m_RenderGroupUtility.renderGroupCount()) throw IndexOutOfBoundsExcept("Index");
		return &(m_RenderGroupUtility.renderGroups()[Index]->Material);
	}//material

	void InstanceActor::render(RenderDevice* pRDev) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");

		m_VertexArray.bind();

		for (auto i : m_RenderGroupUtility.renderGroups()) {
			if (i->pShader == nullptr) continue;

			if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
				pRDev->activeShader(pRDev->shadowPassShader());
			}
			else {
				pRDev->activeShader(i->pShader);
				pRDev->activeMaterial(&i->Material);
			}
			glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBuffer.size() / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
		}//for[all render groups]

	}//render

}