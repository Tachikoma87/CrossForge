#include <glad/glad.h>
#include <crossforge/Graphics/RenderDevice.h>
#include <crossforge/Math/CForgeMath.h>
#include "AdaptiveSkeletalActor.h"

using namespace Eigen;

namespace CForge {

	AdaptiveSkeletalActor::AdaptiveSkeletalActor(void) {
		m_FeetAlignmentToggle = true;
		m_LastAdaptationAngle = 0.0f;
		m_MaxAdaptationDelta = 0.1f;
	}//Constructor

	AdaptiveSkeletalActor::~AdaptiveSkeletalActor(void) {

	}//Destructor

	void AdaptiveSkeletalActor::init(T3DMesh<float>* pMesh, SkeletalAnimationController* pController, bool PrepareCPUSkinning) {

		m_FeetAlignmentToggle = true;
		m_LastAdaptationAngle = 0.0f;
		m_MaxAdaptationDelta = 0.2f;

		SkeletalActor::init(pMesh, pController, true);

		//m_SkeletalSkinning.init(pMesh, pController);
		m_Joints = pController->retrieveSkeleton();

		// estimate control parameters
		// find lowest y-coordinate
		float MinY = pMesh->vertex(0).y();
		for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
			if (pMesh->vertex(i).y() < MinY) MinY = pMesh->vertex(i).y();
		}
		pMesh->computeAxisAlignedBoundingBox();
		const float Height = pMesh->aabb().max().y() - pMesh->aabb().min().y();
		float Delta = 0.4f; // Height * 0.005f;

		std::vector<int32_t> Candidates;
		for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
			if (pMesh->vertex(i).y() < MinY + Delta) Candidates.push_back(i);
		}

		// from candidates find points with min/max x and z 
		m_ControlParams.VertexIDLeftForefoot = Candidates[0];
		m_ControlParams.VertexIDLeftHeel = Candidates[0];
		m_ControlParams.VertexIDRightForefoot = Candidates[0];
		m_ControlParams.VertexIDRightHeel = Candidates[0];

		for (auto i : Candidates) {
			Vector3f v = pMesh->vertex(i);
			if (v.x() > 0.0f && v.z() < pMesh->vertex(m_ControlParams.VertexIDLeftHeel).z()) m_ControlParams.VertexIDLeftHeel = i;
			if (v.x() < 0.0f && v.z() < pMesh->vertex(m_ControlParams.VertexIDRightHeel).z()) m_ControlParams.VertexIDRightHeel = i;

			if (v.x() > 0.0f && v.z() > pMesh->vertex(m_ControlParams.VertexIDLeftForefoot).z()) m_ControlParams.VertexIDLeftForefoot = i;
			if (v.x() < 0.0f && v.z() > pMesh->vertex(m_ControlParams.VertexIDRightForefoot).z()) m_ControlParams.VertexIDRightForefoot = i;
		}

		// find LeftAnkle, RightAnkle
		for (uint32_t i = 0; i < m_Joints.size(); ++i) {
			if (m_Joints[i]->Name.compare("LeftAnkle") == 0) m_ControlParams.LeftAnkleJointID = i;
			if (m_Joints[i]->Name.compare("RightAnkle") == 0) m_ControlParams.RightAnkleJointID = i;
			if (m_Joints[i]->Name.compare("Hips") == 0) m_ControlParams.HipJointID = i;
		}

		// find lowest y coordinate of heels over the course of the animation
		float MinYLeftHeel = 100.0f;
		float MinYRightHeel = 100.0f;

		m_pActiveAnimation = m_pAnimationController->createAnimation(0, 1.0f, 0.0f);
		while (m_pActiveAnimation->t < m_pActiveAnimation->Duration) {
			m_pActiveAnimation->t += 1.0f / 60.0f;
			m_pAnimationController->applyAnimation(m_pActiveAnimation, false);
			m_pAnimationController->retrieveSkeleton(&m_Joints);

			//Vector3f P = m_SkeletalSkinning.transformVertex(m_ControlParams.VertexIDLeftHeel);
			Vector3f P = transformVertex(m_ControlParams.VertexIDLeftHeel);
			if (P.y() < MinYLeftHeel) MinYLeftHeel = P.y();
			//P = m_SkeletalSkinning.transformVertex(m_ControlParams.VertexIDRightHeel);
			P = transformVertex(m_ControlParams.VertexIDRightHeel);
			if (P.y() < MinYRightHeel) MinYRightHeel = P.y();
		}

		float GlobalCorrection = (MinYLeftHeel > MinYRightHeel) ? MinYLeftHeel : MinYRightHeel;

		m_GlobalYCorrection = (GlobalCorrection > 0.0f) ? GlobalCorrection : 0.0f;

		m_LastAdaptationAngle = 0.0f;

		//printf("Min Heel Positions: %.2f | %.2f || GlobalYCorrection: %.2f\n", MinYLeftHeel, MinYRightHeel, m_GlobalYCorrection);

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


		if (m_FeetAlignmentToggle) {
			// apply global y correction if necessary
			if (m_GlobalYCorrection > 0.0f) {
				m_Joints[m_ControlParams.HipJointID]->LocalPosition.y() -= m_GlobalYCorrection;
				m_pAnimationController->setSkeletonValues(&m_Joints, true);
				m_pAnimationController->retrieveSkeleton(&m_Joints);
			}

			alignFeetToGround();
		}//if[feet alignment active]

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


	void AdaptiveSkeletalActor::alignFeetToGround(void) {
		if (m_ControlParams.HipJointID < 0) return;
		if (m_ControlParams.LeftAnkleJointID < 0) return;
		if (m_ControlParams.RightAnkleJointID < 0) return;

		static int32_t c = 0;
		// align feet with ground
		// check left foot heel
		Vector3f HeelPos = transformVertex(m_ControlParams.VertexIDLeftHeel);
		if (HeelPos.y() < 0.0f) {
			m_Joints[m_ControlParams.HipJointID]->LocalPosition.y() -= HeelPos.y();
			m_pAnimationController->setSkeletonValues(&m_Joints, true);
			m_pAnimationController->retrieveSkeleton(&m_Joints);

		}
		// check right heel
		HeelPos = transformVertex(m_ControlParams.VertexIDRightHeel);
		if (HeelPos.y() < 0.0f) {
			m_Joints[m_ControlParams.HipJointID]->LocalPosition.y() -= HeelPos.y();
			m_pAnimationController->setSkeletonValues(&m_Joints, true);
			m_pAnimationController->retrieveSkeleton(&m_Joints);
		}

		Quaternionf OriginalRotationLeft = m_Joints[m_ControlParams.LeftAnkleJointID]->LocalRotation;
		Quaternionf OriginalRotationRight = m_Joints[m_ControlParams.RightAnkleJointID]->LocalRotation;

		Vector2f SearchRange = Vector2f(0.0f, 0.0f);
		Quaternionf R = Quaternionf::Identity();
		Vector3f FFPos = transformVertex(m_ControlParams.VertexIDLeftForefoot);
		Quaternionf OriginalRotation = m_Joints[m_ControlParams.LeftAnkleJointID]->LocalRotation;
		float Angle = 0.0f;
		//m_LastFootAdaptation = 0;
		int32_t ThisFootAdapation = 0;

		// correct left forefoot
		if (FFPos.y() < 0.0f) {
			// linear search steps
			while (FFPos.y() < 0.0f) {
				SearchRange.x() = SearchRange.y();
				SearchRange.y() -= 1.0f; // steps of 1 degree

				R = AngleAxisf(CForgeMath::degToRad(SearchRange.y()), Eigen::Vector3f::UnitX());
				m_Joints[m_ControlParams.LeftAnkleJointID]->LocalRotation = R * OriginalRotation;
				m_pAnimationController->setSkeletonValues(&m_Joints, false);
				FFPos = transformVertex(m_ControlParams.VertexIDLeftForefoot);
			}

			// binary search steps
			Angle = SearchRange.x() + (SearchRange.y() - SearchRange.x()) / 2.0f;
			for (uint8_t i = 0; i < 5; i++) {
				Angle = SearchRange.x() + (SearchRange.y() - SearchRange.x()) / 2.0f;
				R = AngleAxisf(CForgeMath::degToRad(Angle), Eigen::Vector3f::UnitX());
				m_Joints[m_ControlParams.LeftAnkleJointID]->LocalRotation = R * OriginalRotation;
				m_pAnimationController->setSkeletonValues(&m_Joints, false);
				FFPos = transformVertex(m_ControlParams.VertexIDLeftForefoot);

				(FFPos.y() < 0.0f) ? SearchRange.x() = Angle : SearchRange.y() = Angle;
			}
			m_pAnimationController->setSkeletonValues(&m_Joints);
			//m_LastFootAdaptation = 1;
			ThisFootAdapation = 1;

			//printf("Correction of left forefoot by %.2f degree!\n", Angle);
		}//correct left forefoot

		SearchRange = Vector2f(0.0f, 0.0f);
		R = Quaternionf::Identity();
		FFPos = transformVertex(m_ControlParams.VertexIDRightForefoot);
		OriginalRotation = m_Joints[m_ControlParams.RightAnkleJointID]->LocalRotation;

		// correct left forefoot
		if (FFPos.y() < 0.0f) {
			// linear search steps
			while (FFPos.y() < 0.0f) {
				SearchRange.x() = SearchRange.y();
				SearchRange.y() -= 1.0f; // steps of 1 degree

				R = AngleAxisf(CForgeMath::degToRad(SearchRange.y()), Eigen::Vector3f::UnitX());
				m_Joints[m_ControlParams.RightAnkleJointID]->LocalRotation = R * OriginalRotation;
				m_pAnimationController->setSkeletonValues(&m_Joints, false);
				FFPos = transformVertex(m_ControlParams.VertexIDRightForefoot);
			}

			// binary search steps
			Angle = SearchRange.x() + (SearchRange.y() - SearchRange.x()) / 2.0f;
			for (uint8_t i = 0; i < 5; i++) {
				Angle = SearchRange.x() + (SearchRange.y() - SearchRange.x()) / 2.0f;
				R = AngleAxisf(CForgeMath::degToRad(Angle), Eigen::Vector3f::UnitX());
				m_Joints[m_ControlParams.RightAnkleJointID]->LocalRotation = R * OriginalRotation;
				m_pAnimationController->setSkeletonValues(&m_Joints, false);
				FFPos = transformVertex(m_ControlParams.VertexIDRightForefoot);

				(FFPos.y() < 0.0f) ? SearchRange.x() = Angle : SearchRange.y() = Angle;
			}
			m_pAnimationController->setSkeletonValues(&m_Joints);
			//m_LastFootAdaptation = 2;
			ThisFootAdapation = 2;
			//printf("Correction of right forefoot by %.2f degree!\n", Angle);
		}//correct right forefoot

		// smooth fade out required?
		static int32_t Counter = 0;
		if (m_LastFootAdaptation != 0 && std::abs(m_LastAdaptationAngle - Angle) > m_MaxAdaptationDelta && (m_LastAdaptationAngle - Angle < 0.0f)) {

			m_LastAdaptationAngle += ((m_LastAdaptationAngle - Angle > 0.0f) ? -1.0f : 1.0f) * m_MaxAdaptationDelta;
			if (m_LastFootAdaptation == 1) {
				R = AngleAxisf(CForgeMath::degToRad(m_LastAdaptationAngle), Eigen::Vector3f::UnitX());
				m_Joints[m_ControlParams.LeftAnkleJointID]->LocalRotation = R * OriginalRotationLeft;
				m_pAnimationController->setSkeletonValues(&m_Joints, true);
			}
			else {
				R = AngleAxisf(CForgeMath::degToRad(m_LastAdaptationAngle), Eigen::Vector3f::UnitX());
				m_Joints[m_ControlParams.RightAnkleJointID]->LocalRotation = R * OriginalRotationRight;
				m_pAnimationController->setSkeletonValues(&m_Joints, true);
			}
			//printf("Fade out active: %d - %.2f/%.2f\n", Counter++, m_LastAdaptationAngle, Angle);
		}
		else {
			m_LastAdaptationAngle = Angle;
			m_LastFootAdaptation = ThisFootAdapation;
		}

	}//alignFeetToGround

	void AdaptiveSkeletalActor::feetAlignment(bool Active) {
		m_FeetAlignmentToggle = Active;
	}//feetAlignment

	bool AdaptiveSkeletalActor::feetAlignment(void)const {
		return m_FeetAlignmentToggle;
	}//feetAlignment

}//name space