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

		/*if (m_ViewFrustum.visible(m_BoundingSphere, Rot, Pos, S)) pRDev->requestRendering(m_pRenderable, Rot, Pos, S);
		else m_Culled++;*/

		if(m_BV.type() == BoundingVolume::TYPE_UNKNOWN) pRDev->requestRendering(m_pRenderable, Rot, Pos, S);
		else if (m_ViewFrustum.visible(m_AABB, Rot, Pos, S)) pRDev->requestRendering(m_pRenderable, Rot, Pos, S);
		else m_Culled++;

		//pRDev->requestRendering(m_pRenderable, Rot, Pos, S);
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

	void SGNCullingGeom::boundingVolume(const BoundingVolume* pBV) {
		if (nullptr == pBV) m_BV.clear();
		else m_BV = (*pBV);
	}//boundingVolume

	BoundingVolume SGNCullingGeom::boundingVolume(void)const {
		return m_BV;
	}//boundingVolume

}//name space