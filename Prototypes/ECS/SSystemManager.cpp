#include "SSystemManager.h"
#include <crossforge/Core/SLogger.h>

namespace CForge {
	std::shared_ptr<SSystemManager> SSystemManager::m_instance = nullptr;

	std::shared_ptr<SSystemManager> SSystemManager::getInstance() {
		if (nullptr == m_instance) {
			SSystemManager* pSysMan = new SystemManager();
			m_instance = std::make_shared<SSystemManager>(*pSysMan);
		}
		return m_instance;
	}
	void SSystemManager::clear() {
		m_instance = nullptr;
	}

	bool SSystemManager::hasSystem(const std::string identification) {
		return (m_systemsMap.end() != m_systemsMap.find(identification));
	}

	bool SSystemManager::addSystem(SystemBasePtr pSystem) {
		if (nullptr == pSystem) throw NullpointerExcept("pSystem");

		bool result = false;
		if (hasSystem(pSystem->getIdentification())) {
			LogError("System manager already contains " + pSystem->getIdentification(), "");
		}
		else {
			m_systemsMap.insert(std::make_pair(pSystem->getIdentification(), pSystem));
			result = true;
		}
		return result;
	}

	bool SSystemManager::removeSystem(const std::string identification) {
		return (1 == m_systemsMap.erase(identification));
	}

	SSystemManager::~SSystemManager() {
		m_systemsMap.clear();
	}

	SSystemManager::SSystemManager() {
		
	}

	void SSystemManager::initialiize() {
		m_systemsMap.clear();
	}
}