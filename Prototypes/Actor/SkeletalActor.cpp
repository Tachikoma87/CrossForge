#include "../../CForge/Graphics/RenderDevice.h"
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

		m_pAnimationController = pController;

	}//initialize

	void SkeletalActor::clear(void) {
	
	}//clear

	void SkeletalActor::release(void) {
		delete this;
	}//release

	void SkeletalActor::render(RenderDevice* pRDev) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");

		m_pAnimationController->applyAnimation(m_pActiveAnimation);
		
		m_VertexArray.bind();
		// set current animation data 
		// if active animation is nullptr bind pose will be set
		m_pAnimationController->applyAnimation(m_pActiveAnimation, true);

		for (auto i : m_RenderGroupUtility.renderGroups()) {
			if (i->pShader == nullptr) continue;

			if (pRDev->activePass() == RenderDevice::RENDERPASS_SHADOW) {
				pRDev->activeShader(m_pAnimationController->shadowPassShader());
				uint32_t BindingPoint = pRDev->activeShader()->uboBindingPoint(GLShader::DEFAULTUBO_BONEDATA);
				if (BindingPoint != GL_INVALID_INDEX) m_pAnimationController->ubo()->bind(BindingPoint);		
			}else{
				pRDev->activeShader(i->pShader);

				uint32_t BindingPoint = pRDev->activeShader()->uboBindingPoint(GLShader::DEFAULTUBO_BONEDATA);
				if(BindingPoint != GL_INVALID_INDEX) m_pAnimationController->ubo()->bind(BindingPoint);

				pRDev->activeMaterial(&i->Material);
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