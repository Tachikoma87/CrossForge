#include "AssociatedWindowComponent.h"

namespace crossforge {

	AssociatedWindowComponent::AssociatedWindowComponent() : ComponentBase(AssociatedWindowComponent::identification){
		m_inheritance.push_back(AssociatedWindowComponent::identification);
		m_windowEntityId = 0;
	}
	AssociatedWindowComponent::~AssociatedWindowComponent() {

	}

	int64_t& AssociatedWindowComponent::windowEntityId() {
		return m_windowEntityId;
	}

}