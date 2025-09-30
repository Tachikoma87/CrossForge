#include "ControllerBase.h"

namespace crossforge {

	std::string ControllerBase::getIdentification()const {
		return m_inheritance[m_inheritance.size() - 1];
	}
	bool ControllerBase::isInstanceOf(const std::string identification) {
		bool result = false;

		for (int32_t i = int32_t(m_inheritance.size()) - 1; i >= 0; --i) {
			if (0 == m_inheritance[i].compare(identification)) {
				result = true;
				break;
			}
		}
		return result;
	}

	std::vector<std::string> ControllerBase::getInheritance()const {
		return m_inheritance;
	}

	ControllerBase::~ControllerBase() {

	}
	ControllerBase::ControllerBase(const std::string identification) {
		m_inheritance.push_back(identification);
		m_inheritance.push_back(ControllerBase::identification);
	}
}