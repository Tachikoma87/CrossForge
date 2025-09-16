#include "SEntityManager.h"

namespace CForge {
	std::shared_ptr<SEntityManager> SEntityManager::m_instance = nullptr;

	std::shared_ptr<SEntityManager> SEntityManager::Instance() {
		if (nullptr == m_instance) {
			SEntityManager* pMan = new SEntityManager();
			m_instance = std::make_shared<SEntityManager>(*pMan);
		}
		return m_instance;
	}

	void SEntityManager::Clear() {
		m_instance = nullptr;
	}


	int64_t SEntityManager::RegisterEntity(EntityBasePtr entity) {
		int64_t result = -1;
		if (entity->GetEntityId() >= 0) {
			LogError("Entity " + std::to_string(entity->GetEntityId()) + " of type " + entity->GetIdentification() + " is already registered!", "");
		}
		else {
			result = (int64_t) m_entities.size();
			m_entities.push_back(entity);
		}
		return result;
	}

	bool SEntityManager::UnregisterEntity(EntityBasePtr entity) {
		bool result = false;
		if (entity->GetEntityId() < 0) {
			LogWarning("Given entity of type " + entity->GetIdentification() + " has entity id of -1 and can not be unregistered!", "");
		}
		else if (nullptr == m_entities[entity->GetEntityId()]) {
			LogWarning("Entity " + std::to_string(entity->GetEntityId()) + " of type " + entity->GetIdentification() + " is already unregistered!", "");
		}
		else {
			m_entities[entity->GetEntityId()] = nullptr;
			result = true;
		}
		return result;
	}


	SEntityManager::SEntityManager() {

	}

	SEntityManager::~SEntityManager() {
		m_entities.clear();
	}

	void SEntityManager::Initialize() {
		m_entities.clear();

	}
	

}