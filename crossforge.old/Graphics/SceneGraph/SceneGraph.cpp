#include "SceneGraph.h"

using namespace Eigen;

namespace CForge {


	SceneGraph::SceneGraph(void): CForgeObject("SceneGraph") {
		m_pRoot = nullptr;
	}//Constructor

	SceneGraph::~SceneGraph(void) {
		clear();
	}//Destructor

	void SceneGraph::init(ISceneGraphNode* pRoot) {
		clear();
		m_pRoot = pRoot;
	}//initialize

	void SceneGraph::clear(void) {
		m_pRoot = nullptr;
	}//clear

	void SceneGraph::rootNode(ISceneGraphNode* pNode) {
		m_pRoot = pNode;
	}//rootNode

	ISceneGraphNode* SceneGraph::rootNode(void) {
		return m_pRoot;
	}//rootNode

	void SceneGraph::update(float FPSScale) {
		if (nullptr != m_pRoot) m_pRoot->update(FPSScale);
	}//update

	void SceneGraph::render(RenderDevice* pRDev) {
		if (nullptr == pRDev) throw NullpointerExcept("pRDev");
		if (nullptr != m_pRoot) m_pRoot->render(pRDev, Vector3f::Zero(), Quaternionf::Identity(), Vector3f(1.0f, 1.0f, 1.0f));
	}//render

}//name space