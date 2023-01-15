#include "BoundingVolume.h"

using namespace Eigen;

namespace CForge {

	BoundingVolume::BoundingVolume(void) {
		m_Type = TYPE_UNKNOWN;
	}//Constructor

	BoundingVolume::~BoundingVolume(void) {
		clear();
	}//Destructor

	void BoundingVolume::init(const T3DMesh<float>* pMesh, Type T) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		clear();
		m_AABB.init(pMesh->aabb().Min, pMesh->aabb().Max);
		m_Sphere.init(m_AABB.min() + 0.5f * m_AABB.diagonal(), 0.5f * m_AABB.diagonal().norm());
		m_Type = T;
	}//initialize

	void BoundingVolume::init(const AABB Box) {
		m_AABB = Box;
		m_Sphere.init(m_AABB.min() + 0.5f * m_AABB.diagonal(), 0.5f * m_AABB.diagonal().norm());
		m_Type = TYPE_AABB;
	}//initialize

	void BoundingVolume::init(const BoundingSphere Sphere) {
		m_Sphere = Sphere;
		m_AABB.init(Sphere.center() - Vector3f::Ones() * Sphere.radius(), Sphere.center() + Vector3f::Ones() * Sphere.radius());
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

	AABB BoundingVolume::aabb(void)const {
		return m_AABB;
	}//aabb

	BoundingSphere BoundingVolume::boundingSphere(void)const {
		return m_Sphere;
	}//boundingSphere

	BoundingVolume::Type BoundingVolume::type(void)const {
		return m_Type;
	}//type

}//name space