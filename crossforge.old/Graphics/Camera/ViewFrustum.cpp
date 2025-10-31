#include "../Camera/VirtualCamera.h"
#include "ViewFrustum.h"

using namespace Eigen;

namespace CForge {

	ViewFrustum::ViewFrustum(void): CForgeObject("ViewFrustum") {
		m_pCamera = nullptr;
	}//Constructor

	ViewFrustum::~ViewFrustum(void) {
		clear();
	}//Destructor

	void ViewFrustum::init(VirtualCamera* pCamera) {
		if (nullptr == pCamera) throw NullpointerExcept("pCamera");
		m_pCamera = pCamera;
		update();
	}//initialize

	void ViewFrustum::clear(void) {
		m_pCamera = nullptr;
	}//clear


	void ViewFrustum::update(void) {

		const Vector3f DirMultFar = m_pCamera->farPlane() * m_pCamera->dir();
		const Vector3f CamPos = m_pCamera->position();
		const Vector3f CamDir = m_pCamera->dir();
		const Vector3f CamUp = m_pCamera->up();
		const Vector3f CamRight = m_pCamera->right();

		if (m_pCamera->getFOV() > 0.0f) {
			// perspective projection
			const float HalfVSide = m_pCamera->farPlane() * std::tan(0.5f * m_pCamera->fieldOfView());
			const float Aspect = (float)m_pCamera->viewportWidth() / (float)m_pCamera->viewportHeight();
			const float HalfHSide = HalfVSide * Aspect;
			m_Planes[PLANE_NEAR].init(CamPos + m_pCamera->nearPlane() * CamDir, CamDir);
			m_Planes[PLANE_FAR].init(CamPos + DirMultFar, -CamDir);
			m_Planes[PLANE_RIGHT].init(CamPos, CamUp.cross(DirMultFar + CamRight * HalfHSide));
			m_Planes[PLANE_LEFT].init(CamPos, (DirMultFar - CamRight * HalfHSide).cross(CamUp));
			m_Planes[PLANE_TOP].init(CamPos, CamRight.cross(DirMultFar - CamUp * HalfVSide));
			m_Planes[PLANE_BOTTOM].init(CamPos, (DirMultFar + CamUp * HalfVSide).cross(CamRight));
		}
		else {
			// orthogonal projection
			float Width = (float)m_pCamera->viewportWidth();
			float Height = (float)m_pCamera->viewportHeight();
			m_Planes[PLANE_NEAR].init(CamPos, CamDir);
			m_Planes[PLANE_FAR].init(CamPos + DirMultFar, -CamDir);
			m_Planes[PLANE_RIGHT].init(CamPos + m_pCamera->right() * Width / 2.0f, -CamRight);
			m_Planes[PLANE_LEFT].init(CamPos - m_pCamera->right() * Width / 2.0f, CamRight);
			m_Planes[PLANE_TOP].init(CamPos + m_pCamera->up() * Height / 2.0f, -CamUp);
			m_Planes[PLANE_BOTTOM].init(CamPos - m_pCamera->up() * Height / 2.0f, CamUp);
		}


	}//update

	bool ViewFrustum::visible(const BoundingVolume BV, const Eigen::Quaternionf Rot, const Eigen::Vector3f Trans, const Eigen::Vector3f Scale) const{
		if (BV.type() == BoundingVolume::TYPE_UNKNOWN) throw CForgeExcept("Bounding volume not initialized!");
		bool Rval = true;

		switch (BV.type()) {
		case BoundingVolume::TYPE_AABB: Rval = visible(BV.aabb(), Rot, Trans, Scale); break;
		case BoundingVolume::TYPE_SPHERE: Rval = visible(BV.boundingSphere(), Rot, Trans, Scale); break;
		default: throw CForgeExcept("Not handles bounding volume type encountered!");
		}
		return Rval;
	}//visible

	bool ViewFrustum::visible(const Sphere BS, Quaternionf Rot, Vector3f Trans, Vector3f Scale) const{

		Vector3f GlobalCenter = Rot * Scale.cwiseProduct(BS.center()) + Trans;
		const float MaxScale = std::max(std::max(Scale.x(), Scale.y()), Scale.z());
		const float Radius = BS.radius() * MaxScale;

		for (int8_t i = 0; i < PLANE_COUNT; ++i) {
			const float Dist = m_Planes[i].signedDistance(GlobalCenter);
			if (Dist < -Radius) return false;
		}
		return true;
	}//visible

	bool ViewFrustum::visible(const Box AABB, const Quaternionf Rot, const Vector3f Trans, const Vector3f Scale) const {

		const Vector3f A = Rot * (Scale.cwiseProduct(AABB.min())) + Trans;
		const Vector3f B = Rot * (Scale.cwiseProduct(AABB.max())) + Trans;

		Vector3f Min, Max;
		for (uint8_t i = 0; i < 3; ++i) {
			if (A[i] < B[i]) {
				Min[i] = A[i];
				Max[i] = B[i];
			}
			else {
				Min[i] = B[i];
				Max[i] = A[i];
			}
		}

		for (int8_t i = 0; i < PLANE_COUNT; ++i) {
			//// check each axis (x,y,z) to get the AABB vertex furthest away from the direction the plane is facing (plane normal)
			Vector3f p;
			p.x() = (m_Planes[i].normal().x() < 0.0f) ? Min.x() : Max.x();
			p.y() = (m_Planes[i].normal().y() < 0.0f) ? Min.y() : Max.y();
			p.z() = (m_Planes[i].normal().z() < 0.0f) ? Min.z() : Max.z();
			if (m_Planes[i].signedDistance(p) < 0.0f) return false;

		}//for[all planes]

		return true;
	}//visible


}//name space