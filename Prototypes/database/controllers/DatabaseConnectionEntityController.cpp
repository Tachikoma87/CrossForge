#include "DatabaseConnectionEntityController.h"

#include <pqxx/pqxx>

namespace crossforge {

	DatabaseConnectionEntityController::DatabaseConnectionEntityController(const std::string identification): ControllerBase(DatabaseConnectionEntityController::identification) {
		m_inheritance.push_back(identification);
	}

	DatabaseConnectionEntityController::~DatabaseConnectionEntityController() {

	}

	bool DatabaseConnectionEntityController::openPostgresConnection(DatabaseConnectionEntityPtr pDatabaseConnection) {
		bool result = false;

		auto pDbSettingsComp = pDatabaseConnection->getDatabaseConnectionSettingsComponent();
		if (nullptr == pDbSettingsComp) throw MissingComponentException(DatabaseConnectionSettingsComponent::identification);
		auto pDbConnectionComp = pDatabaseConnection->getPostgresConnectionComponent(true);
		pDbConnectionComp->clear();

		std::string connectionString = "dbname = " + pDbSettingsComp->getDatabaseName() + " user = " + pDbSettingsComp->getUserName() + " password = " + pDbSettingsComp->getUserPassword() + " hostaddr = " + pDbSettingsComp->getHostAddress() + " port = " + std::to_string(pDbSettingsComp->getPort());

		try {
			pDbConnectionComp->connection() = std::make_shared<pqxx::connection>(connectionString);
			if (pDbConnectionComp->connection()->is_open()) result = true;
			else LogError("Connection to database " + pDbSettingsComp->getDatabaseName() + " could be established but is not open.");
		}
		catch (const std::exception& e) {
			LogError("Exception occurred during connection attempt to database " + pDbSettingsComp->getDatabaseName() + " as user " + pDbSettingsComp->getUserName() + " " + e.what());
		}
		catch (...) {
			LogError("Exception occurred during connection attempt to database " + pDbSettingsComp->getDatabaseName() + " as user " + pDbSettingsComp->getUserName());
		}

		if (!result) pDatabaseConnection->removeComponent(PostgresConnectionComponent::identification);

		return result;
	}

	bool DatabaseConnectionEntityController::closePostgresConnection(DatabaseConnectionEntityPtr pDatabaseConnection) {
		if (nullptr == pDatabaseConnection) throw NullpointerExcept("pDatabaseConnection");
		bool result = false;
		
		auto pPostgresConnectionComp = pDatabaseConnection->getPostgresConnectionComponent();
		if (nullptr == pPostgresConnectionComp) result = true;
		else if (pPostgresConnectionComp->connection() != nullptr) {
			try {
				pPostgresConnectionComp->connection()->close();
				pDatabaseConnection->removeComponent(PostgresConnectionComponent::identification);
				result = true;
			}
			catch (std::exception ex) {
				LogError("Exception occurred while closing postgres connection: " + std::string(ex.what()));
			}
		}
		return result;
	}

	bool DatabaseConnectionEntityController::isPostgresConnectionOpen(DatabaseConnectionEntityCPtr pDatabaseConnection) {
		if (nullptr == pDatabaseConnection) throw NullpointerExcept("pDatabaseConnection");
		auto pPostgresConnectionComp = pDatabaseConnection->getPostgresConnectionComponent();
		return (nullptr != pPostgresConnectionComp && nullptr != pPostgresConnectionComp->getConnection() && pPostgresConnectionComp->getConnection()->is_open());
	}


	bool DatabaseConnectionEntityController::executePostgresQuery(DatabaseConnectionEntityPtr pDatabaseConnection, PostgresQueryPtr pQuery) {
		if (nullptr == pDatabaseConnection) throw NullpointerExcept("pDatabaseConnection");
		if (nullptr == pQuery) throw NullpointerExcept("pQuery");
		bool result = false;
		try {
			if (!isPostgresConnectionOpen(pDatabaseConnection) && !openPostgresConnection(pDatabaseConnection)) {
				LogError("Can not establish Postgres connection with provided database connection entity");
			} else {
				auto pTransaction = createPostgresNonTransaction(pDatabaseConnection);
				if (nullptr != pTransaction) {
					pQuery->result() = pTransaction->exec(pQuery->query(), pQuery->params());
					result = true;
				}
				else pQuery->errorMessage() = "Failed to create Postgres non-transaction object.";
			}
		}
		catch (const std::exception e) {
			pQuery->errorMessage() = "Exception occurred: " + std::string(e.what());
		}

		pQuery->wasExecuted() = true;
		return result;
	}

	bool DatabaseConnectionEntityController::executePostgresQueries(DatabaseConnectionEntityPtr pDatabaseConnection) {
		if (nullptr == pDatabaseConnection) throw NullpointerExcept("pDatabaseConnection");
		bool result = false;
		auto pQueriesComp = pDatabaseConnection->getPostgresQueriesComponent();
		if (nullptr == pQueriesComp) throw NullpointerExcept(PostgresQueriesComponent::identification);
		if (!isPostgresConnectionOpen(pDatabaseConnection) && !openPostgresConnection(pDatabaseConnection)) {
			LogError("Can not establish postgres connection with provided database connection entity");
		}
		else {
			auto pTransaction = createPostgresTransaction(pDatabaseConnection);
			if (nullptr == pTransaction) LogError("Failed to create postgres transaction object.");
			else {
				try {
					for (auto pQuery : pQueriesComp->postgresQueries()) {
						pQuery->result() = pTransaction->exec(pQuery->query(), pQuery->params());
						pQuery->wasExecuted() = true;
					}
					pTransaction->commit();
					result = true;
				}
				catch (std::exception& e) {
					LogError("Exception occurred during execution of Postgres queries: " + std::string(e.what()) );
				}
			}
		}
		return result;
	}


	std::shared_ptr<pqxx::transaction<>> DatabaseConnectionEntityController::createPostgresTransaction(DatabaseConnectionEntityPtr pDatabaseConnection) {
		if (nullptr == pDatabaseConnection) throw NullpointerExcept("pDatabaseConnection");
		std::shared_ptr<pqxx::transaction<>> result = nullptr;
		if (!isPostgresConnectionOpen(pDatabaseConnection)) {
			LogError("Database connection not open. can not create transaction object.!");
		}
		else {
			auto pPostgresConnectionComp = pDatabaseConnection->getPostgresConnectionComponent();
			if (nullptr == pPostgresConnectionComp) throw MissingComponentException(PostgresConnectionComponent::identification);
			try {
				result = std::make_shared<pqxx::transaction<>>(*pPostgresConnectionComp->connection());
			}
			catch (std::exception e) {
				LogError("Exception creating Postgres transaction object: " + std::string(e.what()));
			}
		}
		
		return result;
	}

	std::shared_ptr<pqxx::nontransaction> DatabaseConnectionEntityController::createPostgresNonTransaction(DatabaseConnectionEntityPtr pDatabaseConnection) {
		if (nullptr == pDatabaseConnection) throw NullpointerExcept("pDatabaseConnection");
		std::shared_ptr<pqxx::nontransaction> result = nullptr;
		if (!isPostgresConnectionOpen(pDatabaseConnection)) {
			LogError("Database connection not open. can not create transaction object.!");
		}
		else {
			auto pPostgresConnectionComp = pDatabaseConnection->getPostgresConnectionComponent();
			if (nullptr == pPostgresConnectionComp) throw MissingComponentException(PostgresConnectionComponent::identification);
			try {
				result = std::make_shared<pqxx::nontransaction>(*pPostgresConnectionComp->connection());
			}
			catch (std::exception e) {
				LogError("Exception creating Postgres transaction object: " + std::string(e.what()));
			}
		}

		return result;
	}
}