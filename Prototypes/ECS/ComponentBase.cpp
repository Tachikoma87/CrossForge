#include "ComponentBase.h"


namespace CForge {

	ComponentBase::ComponentBase(const std::string identification) {
		m_identification = identification;
	}

	ComponentBase::~ComponentBase() {

	}

	const std::string ComponentBase::getIdentification() {
		return m_identification;
	}
}