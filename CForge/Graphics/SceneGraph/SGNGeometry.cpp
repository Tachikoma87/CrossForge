#include "SGNGeometry.h"

using namespace Eigen;

namespace CForge {
	SGNGeometry::SGNGeometry(void): ISceneGraphNode("SGNGeometry") {
		m_Position = Vector3f::Zero();
		m_Rotation = Quaternionf::Identity();
		m_Scale = Vector3f::Ones();
		m_pRenderable = nullptr;
	}//Constructor

	SGNGeometry::~SGNGeometry(void) {
		clear();
	}//Destructor

	void SGNGeometry::init(ISceneGraphNode *pParent, IRenderableActor* pRenderable, Eigen::Vector3f Position, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale) {
		clear();
		ISceneGraphNode::init(pParent);
		actor(pRenderable);
		position(Position);
		rotation(Rotation);
		scale(Scale);
	}//initialize

	void SGNGeometry::clear(void) {
		ISceneGraphNode::clear();
		m_Position = Vector3f::Zero();
		m_Rotation = Quaternionf::Identity();
		m_Scale = Vector3f::Ones();
		m_pRenderable = nullptr;
	}//clear

	void SGNGeometry::position(Eigen::Vector3f Position) {
		m_Position = Position;
	}//position

	void SGNGeometry::rotation(Eigen::Quaternionf Rotation) {
		m_Rotation = Rotation;
	}//rotation

	void SGNGeometry::scale(Eigen::Vector3f Scale) {
		m_Scale = Scale;
	}//scale

	void SGNGeometry::actor(IRenderableActor* pActor) {
		m_pRenderable = pActor;
	}//actor

	Eigen::Vector3f SGNGeometry::position(void)const {
		return m_Position;
	}//position

	Eigen::Quaternionf SGNGeometry::rotation(void)const {
		return m_Rotation;
	}//rotation

	Eigen::Vector3f SGNGeometry::scale(void)const {
		return m_Scale;
	}//scale

	IRenderableActor* SGNGeometry::actor(void)const {
		return m_pRenderable;
	}//actor

	void SGNGeometry::update(float FPSScale) {
		// nothing to do here
	}//update

	void SGNGeometry::render(RenderDevice* pRDev, Eigen::Vector3f Position, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale) {
		if (nullptr != m_pRenderable && m_RenderingEnabled) {
			Eigen::Vector3f Pos = Position + m_Position;
			Eigen::Quaternionf Rot = Rotation * m_Rotation;
			Eigen::Vector3f S = m_Scale.cwiseProduct(Scale);
			pRDev->requestRendering(m_pRenderable, Rot, Pos, S);
		}
	}//render

}//name space