#include "../RenderDevice.h"
#include <glad/glad.h>
#include "SkeletalActor.h"


namespace CForge {

	SkeletalActor::SkeletalActor(void): IRenderableActor("SkeletalActor", ATYPE_SKELETAL) {
		m_pAnimationController = nullptr;
		m_pActiveAnimation = nullptr;
	}//Constructor

	SkeletalActor::~SkeletalActor(void) {
		clear();
	}//Destructor

	void SkeletalActor::init(T3DMesh<float>* pMesh, SkeletalAnimationController *pController) {
		clear();
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");

		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data!");
		if (pMesh->boneCount() == 0) throw CForgeExcept("Mesh contains no bones!");

		uint16_t VProps = VertexUtility::VPROP_POSITION | VertexUtility::VPROP_BONEINDICES | VertexUtility::VPROP_BONEWEIGHTS;
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

		m_pAnimationController = pController;

	}//initialize

	void SkeletalActor::clear(void) {
	
	}//clear

	void SkeletalActor::release(void) {
		delete this;
	}//release

	void SkeletalActor::render(RenderDevice* pRDev) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");

		if (nullptr != m_pActiveAnimation && m_pActiveAnimation->Finished) {
			m_pAnimationController->destroyAnimation(m_pActiveAnimation);
			m_pActiveAnimation = nullptr;
		}
		
		// set current animation data 
		// if active animation is nullptr bind pose will be set
		m_pAnimationController->applyAnimation(m_pActiveAnimation, true);

		m_VertexArray.bind();
		for (auto i : m_RenderGroupUtility.renderGroups()) {

			switch (pRDev->activePass()) {
			case RenderDevice::RENDERPASS_SHADOW: {
				if (nullptr == i->pShaderShadowPass) continue;
				pRDev->activeShader(m_pAnimationController->shadowPassShader());
				uint32_t BindingPoint = pRDev->activeShader()->uboBindingPoint(GLShader::DEFAULTUBO_BONEDATA);
				if (BindingPoint != GL_INVALID_INDEX) m_pAnimationController->ubo()->bind(BindingPoint);

			}break;
			case RenderDevice::RENDERPASS_GEOMETRY: {
				if (nullptr == i->pShaderGeometryPass) continue;

				pRDev->activeShader(i->pShaderGeometryPass);
				uint32_t BindingPoint = pRDev->activeShader()->uboBindingPoint(GLShader::DEFAULTUBO_BONEDATA);
				if (BindingPoint != GL_INVALID_INDEX) m_pAnimationController->ubo()->bind(BindingPoint);

				pRDev->activeMaterial(&i->Material);

			}break;		
			case RenderDevice::RENDERPASS_FORWARD: {
				if (nullptr == i->pShaderForwardPass) continue;

				pRDev->activeShader(i->pShaderForwardPass);
				uint32_t BindingPoint = pRDev->activeShader()->uboBindingPoint(GLShader::DEFAULTUBO_BONEDATA);
				if (BindingPoint != GL_INVALID_INDEX) m_pAnimationController->ubo()->bind(BindingPoint);

				pRDev->activeMaterial(&i->Material);

			}break;
			}
			
			glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBuffer.size() / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
		}//for[all render groups]
	}//render

	void SkeletalActor::activeAnimation(SkeletalAnimationController::Animation* pAnim) {
		m_pActiveAnimation = pAnim;
	}//activeAnimation

	SkeletalAnimationController::Animation* SkeletalActor::activeAnimation(void)const {
		return m_pActiveAnimation;
	}//activeAnimation

}//name-space