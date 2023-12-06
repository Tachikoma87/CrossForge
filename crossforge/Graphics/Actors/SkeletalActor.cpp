#include "../RenderDevice.h"
#include "../OpenGLHeader.h"
#include "SkeletalActor.h"


namespace CForge {

	SkeletalActor::SkeletalActor(void): IRenderableActor("SkeletalActor", ATYPE_SKELETAL) {
		m_pAnimationController = nullptr;
		m_pActiveAnimation = nullptr;
	}//Constructor

	SkeletalActor::~SkeletalActor(void) {
		clear();
	}//Destructor

	void SkeletalActor::init(T3DMesh<float>* pMesh, SkeletalAnimationController *pController, bool PrepareCPUSkinning) {
		clear();
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");

		if (pMesh->vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data!");
		if (pMesh->boneCount() == 0) throw CForgeExcept("Mesh contains no bones!");

		uint16_t VProps = VertexUtility::VPROP_POSITION | VertexUtility::VPROP_BONEINDICES | VertexUtility::VPROP_BONEWEIGHTS;
		if (pMesh->normalCount() > 0) VProps |= VertexUtility::VPROP_NORMAL;
		if (pMesh->tangentCount() > 0) VProps |= VertexUtility::VPROP_TANGENT;
		if (pMesh->textureCoordinatesCount() > 0) VProps |= VertexUtility::VPROP_UVW;

		if (PrepareCPUSkinning) prepareCPUSkinning(pMesh);

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
		m_BV.init(pMesh, BoundingVolume::TYPE_AABB);
	}//initialize

	void SkeletalActor::prepareCPUSkinning(const T3DMesh<float>* pMesh) {
		// clean up old data
		for (auto& i : m_SkinVertexes) {
			if (nullptr != i) delete i;
		}
		m_SkinVertexes.clear();

		// create vertexes
		for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
			SkinVertex* pSV = new SkinVertex();
			pSV->V = pMesh->vertex(i);
			m_SkinVertexes.push_back(pSV);
		}//for[vertices]

		// go through bones and store influences/weights
		for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
			auto* pBone = pMesh->getBone(i);

			for (uint32_t k = 0; k < pBone->VertexInfluences.size(); ++k) {
				auto* pAffectedSF = m_SkinVertexes[pBone->VertexInfluences[k]];
				pAffectedSF->BoneInfluences.push_back(i);
				pAffectedSF->BoneWeights.push_back(pBone->VertexWeights[k]);
			}//for[vertex influences]
		}//for[all bones]

		// make sure we have at least 4 influences per vertex
		for (auto i : m_SkinVertexes) {
			while (i->BoneInfluences.size() < 4) {
				i->BoneInfluences.push_back(0);
				i->BoneWeights.push_back(0.0f);
			}
		}//for[skin vertexes]
	}//prepareCPUSkinning

	void SkeletalActor::clear(void) {
		for (auto& i : m_SkinVertexes) {
			if (nullptr != i) delete i;
		}
		m_SkinVertexes.clear();

		m_pAnimationController = nullptr;
		m_pActiveAnimation = nullptr;
	}//clear

	void SkeletalActor::release(void) {
		delete this;
	}//release

	void SkeletalActor::render(RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");

		if (nullptr != m_pActiveAnimation && m_pActiveAnimation->Finished) {
			m_pAnimationController->destroyAnimation(m_pActiveAnimation);
			m_pActiveAnimation = nullptr;
		}
		
		// set current animation data 
		// if active animation is nullptr bind pose will be set
		m_pAnimationController->applyAnimation(m_pActiveAnimation, true);

		
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
			default: {

			}break;
			}
			m_VertexArray.bind();
			glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBuffer.size() / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
			m_VertexArray.unbind();
		}//for[all render groups]
	}//render

	void SkeletalActor::activeAnimation(SkeletalAnimationController::Animation* pAnim) {
		m_pActiveAnimation = pAnim;
	}//activeAnimation

	SkeletalAnimationController::Animation* SkeletalActor::activeAnimation(void)const {
		return m_pActiveAnimation;
	}//activeAnimation

	Eigen::Vector3f SkeletalActor::transformVertex(int32_t Index) {
		if (0 == m_SkinVertexes.size()) throw CForgeExcept("Class not prepared for CPU skinning!");
		if (0 > Index || Index >= m_SkinVertexes.size()) throw IndexOutOfBoundsExcept("Index");
		
		auto* pV = m_SkinVertexes[Index];

		const Eigen::Vector4i I = Eigen::Vector4i(pV->BoneInfluences[0], pV->BoneInfluences[1], pV->BoneInfluences[2], pV->BoneInfluences[3]);
		const Eigen::Vector4f W = Eigen::Vector4f(pV->BoneWeights[0], pV->BoneWeights[1], pV->BoneWeights[2], pV->BoneWeights[3]);

		return m_pAnimationController->transformVertex(m_SkinVertexes[Index]->V, I, W);
	}//transformVertex

}//name-space