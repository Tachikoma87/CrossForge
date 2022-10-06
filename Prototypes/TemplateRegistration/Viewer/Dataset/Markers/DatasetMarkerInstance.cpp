#include "DatasetMarkerInstance.h"

namespace TempReg {

	DatasetMarkerInstance::DatasetMarkerInstance() {

	}//Constructor

	DatasetMarkerInstance::~DatasetMarkerInstance() {

	}//Destructor

	void DatasetMarkerInstance::init(CForge::ISceneGraphNode* pParent, DatasetMarkerActor* pMarkerActor, Vector3f Position, Vector3f Scaling) {
		m_TransSGN.init(pParent, Position, Quaternionf::Identity(), Scaling);
		m_GeomSGN.init(&m_TransSGN, pMarkerActor);
	}//init

	void DatasetMarkerInstance::clear(void) {
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
	
	void DatasetMarkerInstance::actor(DatasetMarkerActor* pActor) {
		m_GeomSGN.actor(pActor);
	}//actor

	const DatasetMarkerActor* DatasetMarkerInstance::actor(void) const {
		return (DatasetMarkerActor*)m_GeomSGN.actor();
	}//actor
}