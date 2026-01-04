#include "PrefabComponent.h"

namespace crossforge {

	PrefabComponent::PrefabComponent(): ComponentBase(PrefabComponent::identification) {
		initialize();
	}
	PrefabComponent::~PrefabComponent() {
		clear();
	}
	PrefabComponent::PrefabComponent(const std::string childIdentification): ComponentBase(PrefabComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void PrefabComponent::initialize(const std::shared_ptr<const PrefabComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_pActorPrefab = pRef->m_pActorPrefab;
		}
	}
	void PrefabComponent::clear() {
		m_pActorPrefab = nullptr;
	}


	ActorPrefabEntityPtr& PrefabComponent::actorPrefab() {
		return m_pActorPrefab;
	}

	ActorPrefabEntityCPtr PrefabComponent::getActorPrefab()const {
		return m_pActorPrefab;
	}
	void PrefabComponent::setActorPrefab(ActorPrefabEntityPtr pActorPrefab) {
		m_pActorPrefab = pActorPrefab;
	}
}