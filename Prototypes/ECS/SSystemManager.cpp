#include "SSystemManager.h"
#include <crossforge/Core/SLogger.h>

namespace CForge {
	std::shared_ptr<SSystemManager> SSystemManager::m_instance = nullptr;

	std::shared_ptr<SSystemManager> SSystemManager::GetInstance() {
		if (nullptr == m_instance) {
			SSystemManager* pSysMan = new SystemManager();
			m_instance = std::make_shared<SSystemManager>(*pSysMan);
		}
		return m_instance;
	}
	void SSystemManager::Clear() {
		m_instance = nullptr;
	}

	bool SSystemManager::HasSystem(const std::string identification) {
		return (m_systemsMap.end() != m_systemsMap.find(identification));
	}

	bool SSystemManager::AddSystem(SystemBasePtr pSystem) {
		if (nullptr == pSystem) throw NullpointerExcept("pSystem");

		bool result = false;
		if (HasSystem(pSystem->GetIdentification())) {
			LogError("System manager already contains " + pSystem->GetIdentification(), "");
		}
		else {
			m_systemsMap.insert(std::make_pair(pSystem->GetIdentification(), pSystem));
			result = true;
		}
		return result;
	}

	bool SSystemManager::RemoveSystem(const std::string identification) {
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