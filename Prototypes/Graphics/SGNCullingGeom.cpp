#include "SGNCullingGeom.h"

using namespace Eigen;

namespace CForge {

	uint32_t SGNCullingGeom::m_Culled = 0;

	SGNCullingGeom::SGNCullingGeom(void) {
		m_Culled = 0;
	}//Constructor

	SGNCullingGeom::~SGNCullingGeom(void) {

	}//Destructor

	void SGNCullingGeom::render(RenderDevice* pRDev, const Eigen::Vector3f Position, const Eigen::Quaternionf Rotation, const Eigen::Vector3f Scale) {
		m_ViewFrustum.init(pRDev->activeCamera());

		Eigen::Vector3f Pos = Position + m_Position;
		Eigen::Quaternionf Rot = Rotation * m_Rotation;
		Eigen::Vector3f S = m_Scale.cwiseProduct(Scale);


		// check again BoundingBox
		if (m_ViewFrustum.visible(m_BoundingSphere, Rot, Pos, S)) SGNGeometry::render(pRDev, Position, Rotation, Scale);
		else m_Culled++;

		/*if (m_ViewFrustum.visible(m_AABB, Rot, Pos, S)) SGNGeometry::render(pRDev, Position, Rotation, Scale);
		else m_Culled++;*/

		

	}//render

	void SGNCullingGeom::boundingSphere(BoundingSphere Sphere) {
		m_BoundingSphere = Sphere;
	}//boundingSphere

	void SGNCullingGeom::aabb(AABB aabb) {
		m_AABB = aabb;
	}//aabb

	uint32_t SGNCullingGeom::culled(bool Reset) {
		const uint32_t Rval = m_Culled;
		if (Reset) m_Culled = 0;
		return Rval;
	}

	void SGNCullingGeom::release(void) {
		delete this;
	}//release

}//name space