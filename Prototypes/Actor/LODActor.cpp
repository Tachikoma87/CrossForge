#include <glad/glad.h>
#include "LODActor.h"
#include "../../CForge/Graphics/RenderDevice.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Core/SLogger.h"
#include "../MeshDecimate.h"

namespace CForge {
	LODActor::LODActor(void) : IRenderableActor("LODActor", ATYPE_STATIC) {
		m_TypeID = ATYPE_STATIC;
		m_TypeName = "LODActor";
		m_pSLOD = SLOD::instance();
	}//Constructor

	LODActor::~LODActor(void) {
		clear();
		// do not delete the first object, we do not own its reference
		for (uint32_t i = 1; i < m_LODMeshes.size(); i++) {
			delete m_LODMeshes[i];
		}
		m_pSLOD->release();
	}//Destructor
	
	void LODActor::init(T3DMesh<float>* pMesh, bool isTranslucent) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data");
		
		m_translucent = isTranslucent;
		
		// fetch LOD levels from SLOD if not given
		if (m_LODStages.empty())
			m_LODStages = std::vector<float>(*m_pSLOD->getLevels());

		m_LODMeshes.push_back(pMesh);
		bindLODLevel(0);
		
		// TODO make LOD generation chainable (use generated LOD model for next decimation)
		// TODO parallelise generation
		for (uint32_t i = 1; i < m_LODStages.size(); i++) {
			T3DMesh<float>* pLODMesh = new T3DMesh<float>();
			MeshDecimator::decimateMesh(pMesh, pLODMesh, m_LODStages[i]);
			m_LODMeshes.push_back(pLODMesh);
		}
		
	}//initialize

	void LODActor::init(T3DMesh<float>* pMesh) {
		init(pMesh, true);
	}//initialize

	void LODActor::init(T3DMesh<float>* pMesh, bool isTranslucent, const std::vector<float>& LODStages) {
		// TODO eigene LOD level bauen
		m_LODStages = LODStages;
		init(pMesh, isTranslucent);
	}//initialize

	void LODActor::bindLODLevel(uint32_t level) {
		clear();
		auto pMesh = m_LODMeshes[level];
		
		// exclude this part?
		uint16_t VertexProperties = 0;
		if (pMesh->vertexCount() > 0) VertexProperties |= VertexUtility::VPROP_POSITION;
		if (pMesh->normalCount() > 0) VertexProperties |= VertexUtility::VPROP_NORMAL;
		if (pMesh->tangentCount() > 0) VertexProperties |= VertexUtility::VPROP_TANGENT;
		if (pMesh->textureCoordinatesCount() > 0) VertexProperties |= VertexUtility::VPROP_UVW;
		if (pMesh->colorCount() > 0) VertexProperties |= VertexUtility::VPROP_COLOR;
		// exclude this part?
		
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
			// TODO dont recreate textures, use already existing texture objects from main Mesh
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
	}
	
	void LODActor::clear(void) {
		m_VertexBuffer.clear();
		m_ElementBuffer.clear();
		m_VertexArray.clear();

		m_VertexUtility.clear();
		m_RenderGroupUtility.clear();
	}//Clear

	void LODActor::release(void) {
		delete this;
	}//release

	uint32_t LODActor::materialCount(void) const {
		return m_RenderGroupUtility.renderGroupCount();
	}//materialCount

	RenderMaterial* LODActor::material(uint32_t Index) {
		if (Index >= m_RenderGroupUtility.renderGroupCount()) throw IndexOutOfBoundsExcept("Index");
		return &(m_RenderGroupUtility.renderGroups()[Index]->Material);
	}
	
	void LODActor::render(RenderDevice* pRDev) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");

		m_VertexArray.bind();

		for (auto i : m_RenderGroupUtility.renderGroups()) {
			if (i->pShader == nullptr) continue;

			if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
				pRDev->activeShader(pRDev->shadowPassShader());
			}
			else {
				pRDev->activeMaterial(&i->Material);
				pRDev->activeShader(i->pShader);
			}
			glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBuffer.size() / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
		}//for[all render groups]

	}//render
	
	bool LODActor::isTranslucent() {
		return m_translucent;
	}
}