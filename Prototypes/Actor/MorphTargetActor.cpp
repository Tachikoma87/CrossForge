#include <glad/glad.h>
#include "../../CForge/Graphics/RenderDevice.h"
#include "MorphTargetActor.h"

using namespace Eigen;

namespace CForge {

	MorphTargetActor::MorphTargetActor(void): IRenderableActor("MorphTargetActor", 5) {


	}//Constructor

	MorphTargetActor::~MorphTargetActor(void) {
		clear();
	}//Destructor

	void MorphTargetActor::init(T3DMesh<float>* pMesh) {
		clear();
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data. Can not initialize!");

		uint16_t VProps = VertexUtility::VPROP_POSITION;
		if (pMesh->normalCount() > 0) VProps |= VertexUtility::VPROP_NORMAL;
		if (pMesh->textureCoordinatesCount() > 0) VProps |= VertexUtility::VPROP_UVW;

		m_VertexUtility.init(VProps);
		void* pBuffer = nullptr;
		uint32_t BufferSize = 0;
		m_VertexUtility.buildBuffer(pMesh->vertexCount(), &pBuffer, &BufferSize, pMesh);

		// build vertex buffer
		m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
		delete[] pBuffer;
		pBuffer = nullptr;
		BufferSize = 0;

		m_RenderGroupUtility.init(pMesh, &pBuffer, &BufferSize);
		// build index buffer
		m_ElementBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);

		delete[] pBuffer;
		pBuffer = nullptr;
		BufferSize = 0;

		m_VertexArray.init();
		m_VertexArray.bind();
		setBufferData();
		m_VertexArray.unbind();

		buildMorphTargetBuffer(pMesh);

		m_MorphTargetUBO.init();
		m_MorphTargetUBO.dataOffset(pMesh->vertexCount());

		m_MorphTargetUBO.activeMorphTargets(2);
		//m_MorphTargetUBO.setMorphTargetParam(0, 1, 0.5f);
		m_MorphTargetUBO.setMorphTargetParam(1, 2, 0.7f);

	}//initialize

	void MorphTargetActor::clear(void) {
		m_ElementBuffer.clear();
		m_MorphTargetBuffer.clear();
		m_MorphTargetUBO.clear();
		m_RenderGroupUtility.clear();
		m_VertexArray.clear();
		m_VertexBuffer.clear();
		m_VertexUtility.clear();
	}//clear

	void MorphTargetActor::release(void) {
		delete this;
	}//release

	void MorphTargetActor::render(RenderDevice* pRDev) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");

		m_VertexArray.bind();

		for (auto i : m_RenderGroupUtility.renderGroups()) {
			if (i->pShader == nullptr) continue;

			if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
				
			}
			else {		
				pRDev->activeShader(i->pShader);
				pRDev->activeMaterial(&i->Material);
				int32_t MTTex = i->pShader->uniformLocation(GLShader::DEFAULTTEX_MORPHTARGETDATA);
				if (MTTex >= 0) {
					m_MorphTargetBuffer.bindTextureBuffer(MTTex, GL_RGB32F);
					glUniform1i(MTTex, MTTex);
				}
				int32_t MTUBO = i->pShader->uboBindingPoint(GLShader::DEFAULTUBO_MORPHTARGETDATA);
				if (MTUBO >= 0) m_MorphTargetUBO.bind(MTUBO);
			}
			glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBuffer.size() / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
		}//for[all render groups]
	}//render

	void MorphTargetActor::update(float fpsScale) {

	}//update

	void MorphTargetActor::buildMorphTargetBuffer(T3DMesh<float>* pMesh) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->morphTargetCount() == 0) throw CForgeExcept("Mesh contains no morph targets. Can not build morph target buffer.");

		m_MorphTargetBuffer.clear();

		float* pBufferData = nullptr;
		uint32_t ElementCount = 0;

		// one set of vertices for each morph target
		ElementCount = pMesh->morphTargetCount() * (pMesh->vertexCount() * 3);
		pBufferData = new float[ElementCount];
		// initialize with 0
		CoreUtility::memset(pBufferData, 0.0f, ElementCount);

		for (uint32_t i = 0; i < pMesh->morphTargetCount(); ++i) {
			T3DMesh<float>::MorphTarget* pMT = pMesh->getMorphTarget(i);

			for (uint32_t k = 0; k < pMT->VertexIDs.size(); ++k) {
				const Vector3f Displ = pMT->VertexOffsets[k];
				const uint32_t ID = pMT->VertexIDs[k];
				const uint32_t Index = i * (pMesh->vertexCount() * 3) + (ID * 3);

				pBufferData[Index + 0] = Displ[0];
				pBufferData[Index + 1] = Displ[1];
				pBufferData[Index + 2] = Displ[2];

			}//for[vertexIDs]
		}

		m_MorphTargetBuffer.init(GLBuffer::BTYPE_TEXTURE, GLBuffer::BUSAGE_STATIC_DRAW, pBufferData, ElementCount * sizeof(float));

		// clean up
		delete[] pBufferData;
		pBufferData = nullptr;
	}//buildMorphTargetBuffer

}//name space