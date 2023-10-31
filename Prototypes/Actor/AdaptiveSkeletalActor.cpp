#include <glad/glad.h>
#include <crossforge/Graphics/RenderDevice.h>
#include <crossforge/Math/CForgeMath.h>
#include "AdaptiveSkeletalActor.h"

using namespace Eigen;

namespace CForge {

	AdaptiveSkeletalActor::AdaptiveSkeletalActor(void) {

	}//Constructor

	AdaptiveSkeletalActor::~AdaptiveSkeletalActor(void) {

	}//Destructor

	void AdaptiveSkeletalActor::init(T3DMesh<float>* pMesh, SkeletalAnimationController* pController) {
		SkeletalActor::init(pMesh, pController);

		m_SkeletalSkinning.init(pMesh, pController);
		m_Joints = pController->retrieveSkeleton();
	}//initialize
	void AdaptiveSkeletalActor::render(RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");

		if (nullptr != m_pActiveAnimation && m_pActiveAnimation->Finished) {
			m_pAnimationController->destroyAnimation(m_pActiveAnimation);
			m_pActiveAnimation = nullptr;
		}

		// set current animation data 
		// if active animation is nullptr bind pose will be set
		m_pAnimationController->applyAnimation(m_pActiveAnimation, true);

		m_pAnimationController->retrieveSkeleton(&m_Joints);
		
		Quaternionf R = m_Joints[3]->LocalRotation;

		Quaternionf MR = Quaternionf::Identity();
		MR = AngleAxisf(CForgeMath::degToRad(20.0f), Eigen::Vector3f::UnitY());

		m_Joints[3]->LocalRotation = MR * R;
		m_pAnimationController->setSkeletonValues(&m_Joints);


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

}//name space