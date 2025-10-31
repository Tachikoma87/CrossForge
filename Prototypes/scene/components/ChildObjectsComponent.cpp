#include "ChildObjectsComponent.h"

namespace crossforge {

	ChildObjectsComponent::ChildObjectsComponent(): ComponentBase(ChildObjectsComponent::identification) {

	}
	ChildObjectsComponent::ChildObjectsComponent(const std::string childIdentification): ComponentBase(ChildObjectsComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}
	ChildObjectsComponent::~ChildObjectsComponent() {
		clear();
	}

	void ChildObjectsComponent::initialize() {
		clear();
	}
	void ChildObjectsComponent::clear() {
		m_childSceneObjects.clear();
	}

	std::vector<SceneObjectEntityPtr>& ChildObjectsComponent::childSceneObjects() {
		return m_childSceneObjects;
	}

	bool ChildObjectsComponent::addChild(SceneObjectEntityPtr pSceneObject) {
		bool result = false;
		if (hasChild(pSceneObject)) LogError("Scene object " + std::to_string(pSceneObject->getEntityId()) + " already child of this node.");
		else if (0 > pSceneObject->getEntityId()) LogError("Entity can not be added as it an has invalid entity id. It appears not be registered with the entity manager.");
		else {
			m_childSceneObjects.push_back(pSceneObject);
			result = true;
		}
		return result;
	}
	bool ChildObjectsComponent::removeChild(SceneObjectEntityPtr pSceneObject) {
		if (nullptr == pSceneObject) throw NullpointerExcept("pSceneObject");
		bool result = false;

		auto pElementToDelete = m_childSceneObjects.end();
		for (auto i = m_childSceneObjects.begin(); i < m_childSceneObjects.end(); ++i) {
			if ( (*i)->getEntityId() == pSceneObject->getEntityId()) {
				pElementToDelete = i;
				break;
			}
		}
		if (m_childSceneObjects.end() != pElementToDelete) {
			m_childSceneObjects.erase(pElementToDelete);
			result = true;
		}
		else LogWarning("Scene object " + std::to_string(pSceneObject->getEntityId()) + " could not be found. Unable to delete.");
		return result;
	}
	bool ChildObjectsComponent::hasChild(SceneObjectEntityPtr pSceneObject) {
		if (nullptr == pSceneObject) throw NullpointerExcept("pSceneObject");
		bool result = false;
		for (auto i : m_childSceneObjects) {
			if (i->getEntityId() == pSceneObject->getEntityId()) {
				result = true;
				break;
			}
		}
		return result;
	}
}