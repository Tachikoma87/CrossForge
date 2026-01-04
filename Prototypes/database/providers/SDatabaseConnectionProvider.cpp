#include "SDatabaseConnectionProvider.h"
#include "../controllers/DatabaseConnectionEntityController.h"

namespace crossforge {
	std::shared_ptr<SDatabaseConnectionProvider> SDatabaseConnectionProvider::m_pInstance = nullptr;
	std::mutex SDatabaseConnectionProvider::m_mutex;

	SDatabaseConnectionProvider::SDatabaseConnectionProvider() {

	}
	SDatabaseConnectionProvider::~SDatabaseConnectionProvider() {

	}


	std::shared_ptr<SDatabaseConnectionProvider> SDatabaseConnectionProvider::instance() {
		if (nullptr == m_pInstance) {
			auto pInst = new SDatabaseConnectionProvider();
			m_pInstance = std::make_shared<SDatabaseConnectionProvider>(*pInst);
		}
		return m_pInstance;
	}
	void SDatabaseConnectionProvider::destroy() {
		m_pInstance = nullptr;
	}


	bool SDatabaseConnectionProvider::registerDatabaseConnection(std::string connectionName, DatabaseConnectionEntityPtr pDatabaseConnection) {
		if (nullptr == pDatabaseConnection) throw NullpointerExcept("pDatabaseConnection");
		bool result = false;
		if (isConnectionRegistered(connectionName)) LogError("Database connection with name " + connectionName + " already exists. You have to unregister it first to replace it.");
		else {
			this->m_registeredConnectionsMap.insert(std::pair(connectionName, pDatabaseConnection));
			result = true;
		}
		return result;
	}
	bool SDatabaseConnectionProvider::unregisterDatabaseConnection(std::string connectionName) {
		bool result = false;
		auto item = this->m_registeredConnectionsMap.find(connectionName);
		if (m_registeredConnectionsMap.end() == item) {
			LogError("Could not find database connection with name " + connectionName);
		}
		else {
			m_registeredConnectionsMap.erase(item);
			result = true;
		}
		return result;
	}
	bool SDatabaseConnectionProvider::isConnectionRegistered(std::string connectionName) {
		return (m_registeredConnectionsMap.end() != m_registeredConnectionsMap.find(connectionName));
	}

	DatabaseConnectionEntityPtr SDatabaseConnectionProvider::getDatabaseConnection(std::string connectionName) {
		DatabaseConnectionEntityPtr pResult = nullptr;
		
		if (connectionName.empty()) {
			LogError("Empty database connection name specified.");
		}
		else if (!isConnectionRegistered(connectionName)) {
			LogError("A database connection with name " + connectionName + " is not registered.");
		}
		else {
			
			std::lock_guard<std::mutex> lock(m_mutex);
			try {
				auto pConnections = m_managedConnections[connectionName];
				for (auto pCon : pConnections) {
					if (pCon.use_count() == 3) {
						pResult = pCon;
						break;
					}
				}
				if (nullptr == pResult) {
					pResult = std::make_shared<DatabaseConnectionEntity>();
					pResult->getDatabaseConnectionSettingsComponent(true)->initialize(m_registeredConnectionsMap[connectionName]->getDatabaseConnectionSettingsComponent());
					if (DatabaseConnectionEntityController::openPostgresConnection(pResult)) {
						m_managedConnections[connectionName].push_back(pResult);
					}
					else {
						LogError("Failed to establish database connection for " + connectionName);
						pResult = nullptr;
					}
				}
			}
			catch (const std::exception& e) {
				LogError("Exception occurred during generation of database connection: " + std::string(e.what()));
			}
			catch (...) {
				LogError("Not handled exception occurred during database connection retrieval.");
			}

		}
		return pResult;
	}

}