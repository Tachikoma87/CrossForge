#include "ViewFrustum.h"

using namespace Eigen;

namespace CForge {

	ViewFrustum::ViewFrustum(void): CForgeObject("ViewFrustum") {

	}//Constructor

	ViewFrustum::~ViewFrustum(void) {

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
		const float HalfVSide = m_pCamera->farPlane() * std::tan(0.5f * m_pCamera->fieldOfView());
		const float Aspect = (float)m_pCamera->viewportWidth() / (float)m_pCamera->viewportHeight();
		const float HalfHSide = HalfVSide * Aspect;
		const Vector3f DirMultFar = m_pCamera->farPlane() * m_pCamera->dir();
		const Vector3f CamPos = m_pCamera->position();
		const Vector3f CamDir = m_pCamera->dir();
		const Vector3f CamUp = m_pCamera->up();
		const Vector3f CamRight = m_pCamera->right();

		m_Planes[PLANE_NEAR].init(CamPos + m_pCamera->nearPlane() * CamDir, CamDir);
		m_Planes[PLANE_FAR].init(CamPos + DirMultFar, -CamDir);
		m_Planes[PLANE_RIGHT].init(CamPos, CamUp.cross(DirMultFar + CamRight* HalfHSide));
		m_Planes[PLANE_LEFT].init(CamPos, (DirMultFar - CamRight * HalfHSide).cross(CamUp));
		m_Planes[PLANE_TOP].init(CamPos, CamRight.cross(DirMultFar - CamUp * HalfVSide));
		m_Planes[PLANE_BOTTOM].init(CamPos, (DirMultFar + CamUp * HalfVSide).cross(CamRight));

		/*const float Aspect = (float)m_pCamera->viewportWidth() / (float)m_pCamera->viewportHeight();
		float Tang = (float)std::tan(0.5f * m_pCamera->fieldOfView());
		float nh = m_pCamera->nearPlane() * Tang;
		float nw = nh * Aspect;
		float fh = m_pCamera->farPlane() * Tang;
		float fw = fh * Aspect;


		Vector3f z = -m_pCamera->dir();*/
	}//update

	bool ViewFrustum::visible(const BoundingSphere Sphere, Quaternionf Rot, Vector3f Trans, Vector3f Scale) {


		const Vector3f GlobalCenter = Rot * Scale.cwiseProduct(Sphere.center()) + Rot * Trans;
		const float MaxScale = std::max(std::max(Scale.x(), Scale.y()), Scale.z());
		const float Radius = Sphere.radius() * MaxScale;

		for (int8_t i = 0; i < PLANE_COUNT; ++i) {
			if (!sphereInFrustum(m_Planes[i], GlobalCenter, Radius)) 
				return false;

		
		}
		return true;
	}//visible

	bool ViewFrustum::visible(const AABB BoundingBox, const Quaternionf Rot, const Vector3f Trans, const Vector3f Scale) {

		Vector3f Min = Rot * (Scale.cwiseProduct(BoundingBox.min())) + Rot * Scale.cwiseProduct(Trans);
		Vector3f Max = Rot * (Scale.cwiseProduct(BoundingBox.max())) + Rot * Scale.cwiseProduct(Trans);

		for (int8_t i = 0; i < PLANE_COUNT; ++i) {

			// check each axis (x,y,z) to get the AABB vertex furthest away from the direction the plane is facing (plane normal)
			Vector3f p, n;

			// x-axis
			p.x() = (m_Planes[i].normal().x() < 0.0f) ? Min.x() : Max.x();
			p.y() = (m_Planes[i].normal().y() < 0.0f) ? Min.y() : Max.y();
			p.z() = (m_Planes[i].normal().z() < 0.0f) ? Min.z() : Max.z();

			// required if test for intersection
			n.x() = (m_Planes[i].normal().x() < 0.0f) ? Max.x() : Min.x();
			n.y() = (m_Planes[i].normal().y() < 0.0f) ? Max.y() : Min.y();
			n.z() = (m_Planes[i].normal().z() < 0.0f) ? Max.z() : Min.z();

			// now we get the signed distance from the AABB vertex that's furthest down the frustum planes normla,
			// and if the signed distance is negative, then the entire bounding box is behind the frustum plane, which means
			// that is should be culled
			//if (m_Planes[i].distanceToPlane(AxisVert) < 0.0f) return false;

			if (m_Planes[i].signedDistance(n) < 0) return true; // intersection

			if (m_Planes[i].signedDistance(p) < 0) return false;

		}//for[all planes]

		return true;
	}//visible

	bool ViewFrustum::sphereInFrustum(const Plane P, const Eigen::Vector3f Center, float Radius) {
		const float Dist = P.signedDistance(Center);
		if (Dist < -Radius) return false;
		return true;
	}//isOnOrBeforePlane


}//name space