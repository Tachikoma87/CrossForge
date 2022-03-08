#include <glad/glad.h>
#include "../RenderDevice.h"
#include "MorphTargetActor.h"

using namespace Eigen;

namespace CForge {

	MorphTargetActor::MorphTargetActor(void): IRenderableActor("MorphTargetActor", 5) {


	}//Constructor

	MorphTargetActor::~MorphTargetActor(void) {
		clear();
	}//Destructor

	void MorphTargetActor::init(T3DMesh<float>* pMesh, MorphTargetAnimationController *pController) {
		clear();
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data. Can not initialize!");

		uint16_t VProps = VertexUtility::VPROP_POSITION;
		if (pMesh->normalCount() > 0) VProps |= VertexUtility::VPROP_NORMAL;
		if (pMesh->textureCoordinatesCount() > 0) VProps |= VertexUtility::VPROP_UVW;

		m_VertexUtility.init(VProps);
		uint8_t* pBuffer = nullptr;
		uint32_t BufferSize = 0;
		m_VertexUtility.buildBuffer(pMesh->vertexCount(), (void**)&pBuffer, &BufferSize, pMesh);

		// build vertex buffer
		m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
		if(nullptr != pBuffer) delete[] pBuffer;
		pBuffer = nullptr;
		BufferSize = 0;

		m_RenderGroupUtility.init(pMesh, (void**)&pBuffer, &BufferSize);
		// build index buffer
		m_ElementBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);

		if(nullptr != pBuffer) delete[] pBuffer;
		pBuffer = nullptr;
		BufferSize = 0;

		m_VertexArray.init();
		m_VertexArray.bind();
		setBufferData();
		m_VertexArray.unbind();

		buildMorphTargetBuffer(pMesh);

		m_MorphTargetUBO.init();
		m_MorphTargetUBO.dataOffset(pMesh->vertexCount());

		m_pAnimationController = pController;
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

		// create UBO data
		m_pAnimationController->apply(&m_ActiveAnimations, &m_MorphTargetUBO);

		for (auto i : m_RenderGroupUtility.renderGroups()) {
			
			switch (pRDev->activePass()) {
			case RenderDevice::RENDERPASS_GEOMETRY: {
				if (nullptr == i->pShaderGeometryPass) continue;

				pRDev->activeShader(i->pShaderGeometryPass);
				pRDev->activeMaterial(&i->Material);
				int32_t MTTex = i->pShaderGeometryPass->uniformLocation(GLShader::DEFAULTTEX_MORPHTARGETDATA);
				if (MTTex >= 0) {
					m_MorphTargetBuffer.bindTextureBuffer(MTTex, GL_RGB32F);
					glUniform1i(MTTex, MTTex);
				}
				int32_t MTUBO = i->pShaderGeometryPass->uboBindingPoint(GLShader::DEFAULTUBO_MORPHTARGETDATA);
				m_MorphTargetUBO.bind(MTUBO);
			}break;
			case RenderDevice::RENDERPASS_SHADOW: {
				if (nullptr == i->pShaderShadowPass) continue;

				pRDev->activeShader(i->pShaderShadowPass);
				pRDev->activeMaterial(&i->Material);
				int32_t MTTex = i->pShaderShadowPass->uniformLocation(GLShader::DEFAULTTEX_MORPHTARGETDATA);
				if (MTTex >= 0) {
					m_MorphTargetBuffer.bindTextureBuffer(MTTex, GL_RGB32F);
					glUniform1i(MTTex, MTTex);
				}
				int32_t MTUBO = i->pShaderShadowPass->uboBindingPoint(GLShader::DEFAULTUBO_MORPHTARGETDATA);
				m_MorphTargetUBO.bind(MTUBO);
			}break;
			case RenderDevice::RENDERPASS_FORWARD: {
				if (nullptr == i->pShaderForwardPass) continue;

				pRDev->activeShader(i->pShaderForwardPass);
				pRDev->activeMaterial(&i->Material);
				int32_t MTTex = i->pShaderForwardPass->uniformLocation(GLShader::DEFAULTTEX_MORPHTARGETDATA);
				if (MTTex >= 0) {
					m_MorphTargetBuffer.bindTextureBuffer(MTTex, GL_RGB32F);
					glUniform1i(MTTex, MTTex);
				}
				int32_t MTUBO = i->pShaderForwardPass->uboBindingPoint(GLShader::DEFAULTUBO_MORPHTARGETDATA);
				m_MorphTargetUBO.bind(MTUBO);
			}break;
			default: continue;
			}//switch[active pass]

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

	int32_t MorphTargetActor::addAnimation(MorphTargetAnimationController::ActiveAnimation* pAnim) {
		if (nullptr == pAnim) throw NullpointerExcept("pAnim");
		int32_t Rval = -1;

		for (size_t i = 0; i < m_ActiveAnimations.size(); ++i) {
			if (m_ActiveAnimations[i] == nullptr) {
				Rval = i;
				break;
			}
		}
		if (Rval == -1) {
			Rval = m_ActiveAnimations.size();
			m_ActiveAnimations.push_back(nullptr);
		}
		m_ActiveAnimations[Rval] = pAnim;

		return Rval;
	}//addAnimation

}//name space