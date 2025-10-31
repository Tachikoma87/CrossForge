#include "PrefabComponent.h"

namespace crossforge {

	PrefabComponent::PrefabComponent(): ComponentBase(PrefabComponent::identification) {

	}
	PrefabComponent::~PrefabComponent() {
		m_pActorPrefab = nullptr;
	}
	PrefabComponent::PrefabComponent(const std::string childIdentification): ComponentBase(PrefabComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}

	ActorPrefabEntityPtr& PrefabComponent::actorPrefab() {
		return m_pActorPrefab;
	}

}