#include "InstanceSGN.h"

using namespace Eigen;

namespace CForge {
	InstanceSGN::InstanceSGN(void) : ISceneGraphNode("SGNGeometry") {
		m_Position = Vector3f::Zero();
		m_Rotation = Quaternionf::Identity();
		m_Scale = Vector3f::Ones();
		m_pRenderable = nullptr;
	}//Constructor

	InstanceSGN::~InstanceSGN(void) {
		clear();
	}//Destructor

	void InstanceSGN::init(ISceneGraphNode* pParent, InstanceActor* pRenderable, Eigen::Vector3f Position, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale) {
		clear();
		ISceneGraphNode::init(pParent);
		actor(pRenderable);
		position(Position);
		rotation(Rotation);
		scale(Scale);
	}//initialize

	void InstanceSGN::buildTansformation(Eigen::Vector3f* pPosition, Eigen::Quaternionf* pRotation, Eigen::Vector3f* pScale) {

	}

	void InstanceSGN::clear(void) {
		ISceneGraphNode::clear();
		m_Position = Vector3f::Zero();
		m_Rotation = Quaternionf::Identity();
		m_Scale = Vector3f::Ones();
		m_pRenderable = nullptr;
	}//clear

	void InstanceSGN::position(Eigen::Vector3f Position) {
		m_Position = Position;
	}//position

	void InstanceSGN::rotation(Eigen::Quaternionf Rotation) {
		m_Rotation = Rotation;
	}//rotation

	void InstanceSGN::scale(Eigen::Vector3f Scale) {
		m_Scale = Scale;
	}//scale

	void InstanceSGN::actor(InstanceActor* pActor) {
		m_pRenderable = pActor;
	}//actor

	Eigen::Vector3f InstanceSGN::position(void)const {
		return m_Position;
	}//position

	Eigen::Quaternionf InstanceSGN::rotation(void)const {
		return m_Rotation;
	}//rotation

	Eigen::Vector3f InstanceSGN::scale(void)const {
		return m_Scale;
	}//scale

	InstanceActor* InstanceSGN::actor(void)const {
		return m_pRenderable;
	}//actor

	void InstanceSGN::update(float FPSScale) {
		// nothing to do here
	}//update

	void InstanceSGN::render(RenderDevice* pRDev, Eigen::Vector3f Position, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale) {
		if (nullptr != m_pRenderable && m_RenderingEnabled) {
			std::cout << "TEST\n";

			Eigen::Vector3f Pos = Position + m_Position;
			Eigen::Quaternionf Rot = Rotation * m_Rotation;
			Eigen::Vector3f Sca = m_Scale.cwiseProduct(Scale);

			Matrix4f R = GraphicsUtility::rotationMatrix(Rot);
			Matrix4f T = GraphicsUtility::translationMatrix(Pos);
			Matrix4f S = GraphicsUtility::scaleMatrix(Sca);
			Matrix4f ModelMat = T * R * S;

			m_pRenderable->addInstance(ModelMat);
			//m_RenderingEnabled = false;
		}
	}//render

}//name space