#include "SGNGeometry.h"
#include "../OpenGLHeader.h"

using namespace Eigen;

namespace CForge {
	SGNGeometry::SGNGeometry(void): ISceneGraphNode("SGNGeometry") {
		m_Position = Vector3f::Zero();
		m_Rotation = Quaternionf::Identity();
		m_Scale = Vector3f::Ones();
		m_pRenderable = nullptr;
		m_VisualizationMode = VISUALIZATION_FILL;
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
		m_VisualizationMode = VISUALIZATION_FILL;
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

	void SGNGeometry::render(RenderDevice* pRDev, const Eigen::Vector3f Position, const Eigen::Quaternionf Rotation, const Eigen::Vector3f Scale) {
		if (nullptr != m_pRenderable && m_RenderingEnabled) {
			const Eigen::Vector3f Pos = Position + m_Position;
			const Eigen::Quaternionf Rot = Rotation * m_Rotation;
			const Eigen::Vector3f S = m_Scale.cwiseProduct(Scale);

			const BoundingVolume BV = m_pRenderable->boundingVolume();

			if (BV.type() == BoundingVolume::TYPE_UNKNOWN || pRDev->activeCamera()->viewFrustum()->visible(BV, Rot, Pos, S)) {
#				ifndef __EMSCRIPTEN__
				if (m_VisualizationMode != VISUALIZATION_FILL) {
					switch (m_VisualizationMode) {
					case VISUALIZATION_WIREFRAME: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
					case VISUALIZATION_POINTS: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
					default: {

					}break;
					}
					glDisable(GL_CULL_FACE);
					pRDev->requestRendering(m_pRenderable, Rot, Pos, S);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnable(GL_CULL_FACE);
				}
				else {
					pRDev->requestRendering(m_pRenderable, Rot, Pos, S);
				}		
				#else			
					pRDev->requestRendering(m_pRenderable, Rot, Pos, S);
				#endif
			}
				

		}
	}//render

	void SGNGeometry::buildTansformation(Eigen::Vector3f* pPosition, Eigen::Quaternionf* pRotation, Eigen::Vector3f* pScale) {
		Vector3f ParentPosition = Eigen::Vector3f::Zero();
		Quaternionf ParentRotation = Eigen::Quaternionf::Identity();
		Vector3f ParentScale = Eigen::Vector3f::Ones();

		if (nullptr != m_pParent) m_pParent->buildTansformation(&ParentPosition, &ParentRotation, &ParentScale);

		if (nullptr != pPosition) (*pPosition) = ParentPosition + m_Position;
		if (nullptr != pRotation) (*pRotation) = ParentRotation * m_Rotation;
		if (nullptr != pScale) (*pScale) = m_Scale.cwiseProduct(ParentScale);

	}//buildTransformation

	void SGNGeometry::visualization(Visualization Mode) {
		m_VisualizationMode = Mode;
	}//visualization

	SGNGeometry::Visualization SGNGeometry::visualization(void)const {
		return m_VisualizationMode;
	}//visualization

}//name space