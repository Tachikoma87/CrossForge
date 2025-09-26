#include "SystemBase.h"

namespace CForge {

	SystemBase::SystemBase(const std::string identification) {
		m_identification = identification;
	}

	SystemBase::~SystemBase() {

	}

	const std::string SystemBase::getIdentification()const {
		return m_identification;
	}

	bool SystemBase::isEntityRegistered(EntityBasePtr pEntity)const {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		bool result = false;
		for (auto pEntity : m_entities) {
			if (pEntity->getEntityId() == pEntity->getEntityId()) {
				result = true;
				break;
			}
		}
		return result;
	}

	bool SystemBase::registerEntity(EntityBasePtr pEntity) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		bool result = false;
		if (isEntityRegistered(pEntity)) {
			LogError("Entity with id " + std::to_string(pEntity->getEntityId()) + " of type " + pEntity->getIdentification() + " is already registered!", "");
		}
		else {
			m_entities.push_back(pEntity);
			result = true;
		}
		return result;
	}
	bool SystemBase::unregisterEntity(EntityBasePtr pEntity) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		for (auto i = m_entities.begin(); i != m_entities.end(); ++i) {
			if ((*i)->getEntityId() == pEntity->getEntityId()) {
				m_entities.erase(i);
				return true;
			}
		}
		return false;
	}
	uint32_t SystemBase::getEntityCount()const {
		return m_entities.size();
	}

}