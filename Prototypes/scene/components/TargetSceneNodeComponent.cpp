#include "TargetSceneNodeComponent.h"

namespace crossforge {

	TargetSceneNodeComponent::TargetSceneNodeComponent(): ComponentBase(TargetSceneNodeComponent::identification) {
		initialize();
	}
	TargetSceneNodeComponent::TargetSceneNodeComponent(const std::string childIdentification): ComponentBase(TargetSceneNodeComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}
	TargetSceneNodeComponent::~TargetSceneNodeComponent() {
		clear();
	}

	void TargetSceneNodeComponent::initialize(const std::shared_ptr<const TargetSceneNodeComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_pTargetEntity = pRef->m_pTargetEntity;
		}
	}
	void TargetSceneNodeComponent::clear() {
		m_pTargetEntity = nullptr;
	}

	SceneNodeEntityPtr& TargetSceneNodeComponent::targetSceneNode() {
		return m_pTargetEntity;
	}

	const SceneNodeEntityCPtr TargetSceneNodeComponent::getTargetSceneNode()const {
		return m_pTargetEntity;
	}
	void TargetSceneNodeComponent::setTargetSceneNode(const SceneNodeEntityPtr pObj) {
		m_pTargetEntity = pObj;
	}
}