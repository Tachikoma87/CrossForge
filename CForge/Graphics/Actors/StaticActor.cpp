#include "../OpenGLHeader.h"

#include "StaticActor.h"
#include "../RenderDevice.h"
#include "../../Core/SLogger.h"
#include "../../Utility/CForgeUtility.h"

namespace CForge {
	StaticActor::StaticActor(void): IRenderableActor("StaticActor", ATYPE_STATIC) {
		m_TypeID = ATYPE_STATIC;
		m_TypeName = "Static Actor";
	}//Constructor

	StaticActor::~StaticActor(void) {
		clear();
	}//Destructor

	void StaticActor::init(const T3DMesh<float>* pMesh) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data");

		uint16_t VertexProperties = 0;
		if (pMesh->vertexCount() > 0) VertexProperties |= VertexUtility::VPROP_POSITION;
		if (pMesh->normalCount() > 0) VertexProperties |= VertexUtility::VPROP_NORMAL;
		if (pMesh->tangentCount() > 0) VertexProperties |= VertexUtility::VPROP_TANGENT;
		if (pMesh->textureCoordinatesCount() > 0) VertexProperties |= VertexUtility::VPROP_UVW;
		if (pMesh->colorCount() > 0) VertexProperties |= VertexUtility::VPROP_COLOR;
		
		// build array buffer of vertex data
		uint8_t* pBuffer = nullptr;
		uint32_t BufferSize = 0;

		std::string ErrorMsg;
		if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			SLogger::log("Not handled OpenGL error occurred before initialization of a Static Actor: " + ErrorMsg, "StaticActor", SLogger::LOGTYPE_ERROR);
		}

		m_VertexArray.init();
		m_VertexArray.bind();	

		if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			SLogger::log("Not handled OpenGL error occurred after creation of vertex array: " + ErrorMsg, "StaticActor", SLogger::LOGTYPE_ERROR);
		}

		try {
			m_VertexUtility.init(VertexProperties);
			m_VertexUtility.buildBuffer(pMesh->vertexCount(), (void**)&pBuffer, &BufferSize, pMesh);
			m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
			// free buffer data
			if(nullptr != pBuffer) delete[] pBuffer;
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

		if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			SLogger::log("Not handled OpenGL error occurred after creation of vertex utility: " + ErrorMsg, "StaticActor", SLogger::LOGTYPE_ERROR);
		}
	
		// build render groups and element array
		try {
			m_RenderGroupUtility.init(pMesh, (void**)&pBuffer, &BufferSize);
			m_ElementBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
			// free buffer data
			if(nullptr != pBuffer) delete[] pBuffer;
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
		
		setBufferData();
		m_VertexArray.unbind();

		
		if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			SLogger::log("Not handled OpenGL error occurred during initialization of a StaticActor: " + ErrorMsg, "StaticActor", SLogger::LOGTYPE_ERROR);
		}
		
		m_BV.init(pMesh, BoundingVolume::TYPE_SPHERE);
	}//initialize

	void StaticActor::clear(void) {
		m_VertexBuffer.clear();
		m_ElementBuffer.clear(); 
		m_VertexArray.clear();

		m_VertexUtility.clear();
		m_RenderGroupUtility.clear();
	}//Clear

	void StaticActor::release(void) {
		delete this;
	}//release

	void StaticActor::render(RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");

		for (auto i : m_RenderGroupUtility.renderGroups()) {

			switch (pRDev->activePass()) {
			case RenderDevice::RENDERPASS_SHADOW: {
				if (nullptr == i->pShaderShadowPass) continue;
				pRDev->activeShader(i->pShaderShadowPass);
				pRDev->activeMaterial(&i->Material);
			}break;
			case RenderDevice::RENDERPASS_GEOMETRY: {
				if (nullptr == i->pShaderGeometryPass) continue;
				pRDev->activeShader(i->pShaderGeometryPass);
				pRDev->activeMaterial(&i->Material);
			}break;
			case RenderDevice::RENDERPASS_FORWARD: {
				if (nullptr == i->pShaderForwardPass) continue;
				pRDev->activeShader(i->pShaderForwardPass);
				pRDev->activeMaterial(&i->Material);
			}break;
			default: {
				// nothing to do
			}break;
			}

			m_VertexArray.bind();
			glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBuffer.size() / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
			m_VertexArray.unbind();
		}//for[all render groups]

		//
	}//render

}