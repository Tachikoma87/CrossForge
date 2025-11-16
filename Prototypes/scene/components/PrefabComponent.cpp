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

	void PrefabComponent::initialize() {
		clear();
	}
	void PrefabComponent::clear() {
		m_pActorPrefab = nullptr;
	}


	ActorPrefabEntityPtr& PrefabComponent::actorPrefab() {
		return m_pActorPrefab;
	}

}