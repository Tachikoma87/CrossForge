#include "ISceneGraphNode.h"

namespace CForge {

	ISceneGraphNode::ISceneGraphNode(const std::string ClassName): CForgeObject("ISceneGraphNode::" + ClassName) {
		m_pParent = nullptr;
		m_Children.clear();
		m_UpdateEnabled = true;
		m_RenderingEnabled = true;
	}//Constructor

	ISceneGraphNode::~ISceneGraphNode(void) {
		clear();
	}//Destructor


	void ISceneGraphNode::init(ISceneGraphNode* pParent) {
		clear();
		parent(pParent);
		m_UpdateEnabled = true;
		m_RenderingEnabled = true;
	}//initialize

	void ISceneGraphNode::clear(void) {
		removeAllChildren();
		parent(nullptr);

		m_Children.clear(); 
		m_pParent = nullptr;
		m_UpdateEnabled = true;
		m_RenderingEnabled = true;
	}//clear

	void ISceneGraphNode::parent(ISceneGraphNode* pNode) {
		if (pNode == m_pParent) return; // nothing to do
		if (m_pParent != nullptr) {
			m_pParent->removeChild(this);
			m_pParent = nullptr;
		}

		m_pParent = pNode;
		if(nullptr != m_pParent) m_pParent->addChild(this);
	}//parent

	ISceneGraphNode* ISceneGraphNode::parent(void)const {
		return m_pParent;
	}//parent

	void ISceneGraphNode::addChild(ISceneGraphNode* pNode) {
		if (nullptr == pNode) throw NullpointerExcept("PNode");

		if (isChild(pNode)) return; // node is already child of this node

		m_Children.push_back(pNode);
		pNode->parent(this);
	}//addChild

	void ISceneGraphNode::removeChild(ISceneGraphNode* pNode) {
		if (nullptr == pNode) throw NullpointerExcept("pNode");
		if (!isChild(pNode)) return;

		m_Children.remove(pNode);
		pNode->parent(nullptr);

	}//removeChild

	void ISceneGraphNode::removeAllChildren(void) {
		std::vector<ISceneGraphNode*> NodesToRemove;
		for (auto i : m_Children) NodesToRemove.push_back(i);
		m_Children.clear();

		for (auto i : NodesToRemove) i->parent(nullptr);
	}//removeAllChildren

	bool ISceneGraphNode::isChild(const ISceneGraphNode* pNode)const {
		if (nullptr == pNode) return false;
		bool Rval = false;
		for (auto i : m_Children) {
			if (pNode == i) {
				Rval = true;
				break;
			}
		}//for[all child nodes]
		return Rval;
	}//isChild

	uint32_t ISceneGraphNode::childCount(void)const {
		return m_Children.size();
	}//childCount

	ISceneGraphNode* ISceneGraphNode::child(uint32_t Index) {
		if (Index > m_Children.size()) throw IndexOutOfBoundsExcept("Index");
		auto Node = m_Children.begin();
		for (uint32_t i = 0; i < Index; ++i, ++Node);
		return (*Node);
	}//child

	void ISceneGraphNode::enable(bool Update, bool Rendering) {
		m_UpdateEnabled = Update;
		m_RenderingEnabled = Rendering;
	}//enable

	void ISceneGraphNode::enabled(bool* pUpdate, bool* pRendering) {
		if (nullptr != pUpdate) (*pUpdate) = m_UpdateEnabled;
		if (nullptr != pRendering) (*pRendering) = m_RenderingEnabled;
	}//enabled

}//name space