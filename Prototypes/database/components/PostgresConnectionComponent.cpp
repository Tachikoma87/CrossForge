#include "PostgresConnectionComponent.h"

namespace crossforge {
	PostgresConnectionComponent::PostgresConnectionComponent(): ComponentBase(PostgresConnectionComponent::identification) {
		initialize();
	}
	PostgresConnectionComponent::PostgresConnectionComponent(const std::string childIdentification): ComponentBase(PostgresConnectionComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}
	PostgresConnectionComponent::~PostgresConnectionComponent() {
		clear();
	}

	void PostgresConnectionComponent::initialize(const std::shared_ptr<const PostgresConnectionComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_pConnection = pRef->m_pConnection;
		}
	}
	void PostgresConnectionComponent::clear() {
		m_pConnection = nullptr;
	}

	/* Accessor */
	std::shared_ptr<pqxx::connection>& PostgresConnectionComponent::connection() {
		return m_pConnection;
	}
	

	/* Getter */
	const std::shared_ptr<const pqxx::connection> PostgresConnectionComponent::getConnection()const {
		return m_pConnection;
	}

	/* Setter */
	void PostgresConnectionComponent::setConnection(const std::shared_ptr<pqxx::connection> pConnection) {
		m_pConnection = pConnection;
	}
}