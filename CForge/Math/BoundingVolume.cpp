#include "BoundingVolume.h"

using namespace Eigen;

namespace CForge {

	BoundingVolume::BoundingVolume(void): CForgeObject("BoundingVolume") {
		m_Type = TYPE_UNKNOWN;
	}//Constructor

	BoundingVolume::~BoundingVolume(void) {
		clear();
	}//Destructor

	void BoundingVolume::init(const T3DMesh<float>* pMesh, Type T) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		clear();

		if (pMesh->aabb().diagonal().norm() < 0.01f) m_AABB = T3DMesh<float>::computeAxisAlignedBoundingBox(pMesh);
		else m_AABB.init(pMesh->aabb().Min, pMesh->aabb().Max);

		m_Sphere.init(m_AABB.min() + 0.5f * m_AABB.diagonal(), 0.5f * m_AABB.diagonal().norm());
		m_Type = T;
	}//initialize

	void BoundingVolume::init(const Box AABB) {
		m_AABB = AABB;
		m_Sphere.init(m_AABB.min() + 0.5f * m_AABB.diagonal(), 0.5f * m_AABB.diagonal().norm());
		m_Type = TYPE_AABB;
	}//initialize

	void BoundingVolume::init(const Sphere BS) {
		m_Sphere = BS;
		m_AABB.init(m_Sphere.center() - Vector3f::Ones() * m_Sphere.radius(), m_Sphere.center() + Vector3f::Ones() * m_Sphere.radius());
		m_Type = TYPE_SPHERE;
	}//initialize

	void BoundingVolume::clear(void) {
		m_AABB.init(Vector3f::Zero(), Vector3f::Zero());
		m_Sphere.init(Vector3f::Zero(), 0.0f);
		m_Type = TYPE_UNKNOWN;
	}//clear

	void BoundingVolume::release(void) {
		delete this;
	}//release

	Box BoundingVolume::aabb(void)const {
		return m_AABB;
	}//aabb

	Sphere BoundingVolume::boundingSphere(void)const {
		return m_Sphere;
	}//boundingSphere

	BoundingVolume::Type BoundingVolume::type(void)const {
		return m_Type;
	}//type

}//name space