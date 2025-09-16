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

	bool EntityBase::HasComponent(const std::string identification) const{
		return (m_componentMap.find(identification) != m_componentMap.end());
	}

	bool EntityBase::AddComponent(ComponentBasePtr component) {
		if (nullptr == component) throw CForgeExcept("Component was nullptr!");

		bool result = false;
		if (HasComponent(component->GetIdentification())) {
			LogWarning("Entity " + std::to_string(m_entityId) + " already has component " + component->GetIdentification(), "");
		}
		else {
			m_componentMap.insert(std::pair(component->GetIdentification(), component));
			result = true;
		}

		return result;
	}

	bool EntityBase::RemoveComponent(const std::string identification) {
		return (1 == m_componentMap.erase(identification));
	}

	void EntityBase::SetEntityId(int64_t entityId) {
		m_entityId = entityId;
	}

	int64_t EntityBase::GetEntityId()const {
		return m_entityId;
	}

	const std::string EntityBase::GetIdentification()const {
		return m_identification;
	}
}
