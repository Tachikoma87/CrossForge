#include "SystemBase.h"

namespace CForge {

	SystemBase::SystemBase(const std::string identification) {
		m_identification = identification;
	}

	SystemBase::~SystemBase() {

	}

	const std::string SystemBase::GetIdentification()const {
		return m_identification;
	}

}