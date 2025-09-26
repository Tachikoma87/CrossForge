#include "SEntityManager.h"

namespace CForge {
	std::shared_ptr<SEntityManager> SEntityManager::m_instance = nullptr;

	std::shared_ptr<SEntityManager> SEntityManager::instance() {
		if (nullptr == m_instance) {
			SEntityManager* pMan = new SEntityManager();
			m_instance = std::make_shared<SEntityManager>(*pMan);
		}
		return m_instance;
	}

	void SEntityManager::clear() {
		m_instance = nullptr;
	}


	int64_t SEntityManager::registerEntity(EntityBasePtr entity) {
		int64_t result = -1;
		if (entity->getEntityId() >= 0) {
			LogError("Entity " + std::to_string(entity->getEntityId()) + " of type " + entity->getIdentification() + " is already registered!", "");
		}
		else {
			result = (int64_t) m_entities.size();
			m_entities.push_back(entity);
		}
		return result;
	}

	bool SEntityManager::unregisterEntity(EntityBasePtr entity) {
		bool result = false;
		if (entity->getEntityId() < 0) {
			LogWarning("Given entity of type " + entity->getIdentification() + " has entity id of -1 and can not be unregistered!", "");
		}
		else if (nullptr == m_entities[entity->getEntityId()]) {
			LogWarning("Entity " + std::to_string(entity->getEntityId()) + " of type " + entity->getIdentification() + " is already unregistered!", "");
		}
		else {
			m_entities[entity->getEntityId()] = nullptr;
			result = true;
		}
		return result;
	}


	SEntityManager::SEntityManager() {

	}

	SEntityManager::~SEntityManager() {
		m_entities.clear();
	}

	void SEntityManager::initialize() {
		m_entities.clear();

	}
	

}