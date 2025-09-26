#include "EntityBase.h"
#include "SEntityManager.h"

namespace CForge {

	

	EntityBase::EntityBase(const std::string identification, int64_t entityId) {
		m_identification = identification;
		m_entityId = entityId;
	}

	EntityBase::~EntityBase() {
		m_componentMap.clear();
	}

	bool EntityBase::hasComponent(const std::string identification) const{
		return (m_componentMap.find(identification) != m_componentMap.end());
	}

	bool EntityBase::addComponent(ComponentBasePtr component) {
		if (nullptr == component) throw CForgeExcept("Component was nullptr!");

		bool result = false;
		if (hasComponent(component->getIdentification())) {
			LogWarning("Entity " + std::to_string(m_entityId) + " already has component " + component->getIdentification(), "");
		}
		else {
			m_componentMap.insert(std::pair(component->getIdentification(), component));
			result = true;
		}

		return result;
	}

	bool EntityBase::removeComponent(const std::string identification) {
		return (1 == m_componentMap.erase(identification));
	}

	void EntityBase::setEntityId(int64_t entityId) {
		m_entityId = entityId;
	}

	int64_t EntityBase::getEntityId()const {
		return m_entityId;
	}

	const std::string EntityBase::getIdentification()const {
		return m_identification;
	}
}
