#include "DatasetMarkerInstance.h"

namespace TempReg {

	DatasetMarkerInstance::DatasetMarkerInstance() {

	}//Constructor

	DatasetMarkerInstance::~DatasetMarkerInstance() {

	}//Destructor

	void DatasetMarkerInstance::init(CForge::ISceneGraphNode* pParent, CForge::StaticActor* pActor, Vector3f Position, Vector3f Scaling) {
		m_TransSGN.init(pParent, Position, Quaternionf::Identity(), Scaling);
		m_GeomSGN.init(&m_TransSGN, pActor);
	}//init

	void DatasetMarkerInstance::clear(void) {
		removeFromSceneGraph();
		m_GeomSGN.clear();
		m_TransSGN.clear();
	}//clear

	void DatasetMarkerInstance::addToSceneGraph(CForge::ISceneGraphNode* pParent) {
		pParent->addChild(&m_TransSGN);
	}//addToSceneGraph

	void DatasetMarkerInstance::removeFromSceneGraph(void) {
		if (m_TransSGN.parent() == nullptr) return; //nothing to do
		m_TransSGN.parent()->removeChild(&m_TransSGN);
	}//removeFromSceneGraph

	void DatasetMarkerInstance::show(bool Show) {
		m_TransSGN.enable(true, Show);
	}//show

	void DatasetMarkerInstance::translation(Vector3f Translation) {
		m_TransSGN.translation(Translation);
	}//translation

	void DatasetMarkerInstance::actor(CForge::StaticActor* pActor) {
		m_GeomSGN.actor(pActor);
	}//actor

	const CForge::IRenderableActor* DatasetMarkerInstance::actor(void) const {
		return m_GeomSGN.actor();
	}//actor
}