#include "DatabaseConnectionSettingsComponent.h"

namespace crossforge {

	DatabaseConnectionSettingsComponent::DatabaseConnectionSettingsComponent(): ComponentBase(DatabaseConnectionSettingsComponent::identification) {
		initialize();
	}

	DatabaseConnectionSettingsComponent::~DatabaseConnectionSettingsComponent() {

	}

	DatabaseConnectionSettingsComponent::DatabaseConnectionSettingsComponent(const std::string childIdentification): ComponentBase(DatabaseConnectionSettingsComponent::identification) {
		m_inheritance.push_back(childIdentification);
		initialize();
	}

	void DatabaseConnectionSettingsComponent::initialize(const std::shared_ptr<const DatabaseConnectionSettingsComponent> pRef) {
		if (this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_databaseName = pRef->getDatabaseName();
			m_userName = pRef->getUserName();
			m_userPassword = pRef->getUserPassword();
			m_hostAdress = pRef->getHostAddress();
			m_port = pRef->getPort();
		}
	}

	void DatabaseConnectionSettingsComponent::clear() {
		m_databaseName = "";
		m_userName = "";
		m_userPassword = "";
		m_hostAdress = "";
		m_port = 0;
	}

	/* Accessor */
	std::string& DatabaseConnectionSettingsComponent::databaseName() {
		return m_databaseName;
	}
	std::string& DatabaseConnectionSettingsComponent::userName() {
		return m_userName;
	}
	std::string& DatabaseConnectionSettingsComponent::userPassword() {
		return m_userPassword;
	}
	std::string& DatabaseConnectionSettingsComponent::hostAddress() {
		return m_hostAdress;
	}
	uint16_t& DatabaseConnectionSettingsComponent::port() {
		return m_port;
	}

	/* Getter */
	const std::string DatabaseConnectionSettingsComponent::getDatabaseName() const {
		return m_databaseName;
	}
	const std::string DatabaseConnectionSettingsComponent::getUserName() const {
		return m_userName;
	}
	const std::string DatabaseConnectionSettingsComponent::getUserPassword() const {
		return m_userPassword;
	}
	const std::string DatabaseConnectionSettingsComponent::getHostAddress() const {
		return m_hostAdress;
	}
	const uint16_t DatabaseConnectionSettingsComponent::getPort()const {
		return m_port;
	}

	/* Setter */
	void DatabaseConnectionSettingsComponent::setDatabaseName(const std::string databaseName) {
		m_databaseName = databaseName;
	}
	void DatabaseConnectionSettingsComponent::setUserName(const std::string userName) {
		m_userName = userName;
	}
	void DatabaseConnectionSettingsComponent::setUserPassword(const std::string userPassword) {
		m_userPassword = userPassword;
	}
	void DatabaseConnectionSettingsComponent::setHostAddress(const std::string hostAddress) {
		m_hostAdress = hostAddress;
	}
	void DatabaseConnectionSettingsComponent::setPort(const uint16_t port) {
		m_port = port;
	}

}