#include "DatabaseVersionDao.h"
#include "../../miscellaneous/MiscUtility.hpp"

namespace crossforge {

	DatabaseVersionDao::DatabaseVersionDao(DatabaseConnectionEntityPtr pDbConnection): DaoBase("version", pDbConnection) {

	}
	DatabaseVersionDao::~DatabaseVersionDao() {

	}

	const std::vector<DatabaseVersionPocoPtr> DatabaseVersionDao::parseResultSet(const pqxx::result resultSet) const {
		std::vector<DatabaseVersionPocoPtr> result;
		result.reserve(resultSet.size());

		for (auto r : resultSet) {
			try {
				auto pItem = std::make_shared<DatabaseVersionPoco>();
				pItem->versionId() = r["version_id"].as<int64_t>();
				pItem->major() = r["major"].as<int32_t>();
				pItem->minor() = r["minor"].as<int32_t>();
				pItem->patch() = r["patch"].as<int32_t>();
				pItem->timestampCreated() = MiscUtility::getTimestampFromIsoTime(r["timestamp_created"].as<std::string>());
				//pItem->timestampCreated() = r["timestamp_created_raw"].as<int64_t>();
				result.push_back(pItem);
			}
			catch (std::exception& e) {
				LogError("Exception parsing result set: " + std::string(e.what()));
			}
		}
		return result;
	}

	bool DatabaseVersionDao::create(const DatabaseVersionPocoPtr& pPoco) const {
		if (nullptr == m_pDbConnection) throw NullpointerExcept("m_pDbConnection");
		bool result = false;

		PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();

		pQuery->query() = "INSERT INTO " + m_tableName + "(major, minor, patch, timestamp_created) VALUES ($1, $2, $3, $4);";
		pQuery->params().append(pPoco->major());
		pQuery->params().append(pPoco->minor());
		pQuery->params().append(pPoco->patch());
		pQuery->params().append(MiscUtility::getTimeISO(GeneralUtility::getTimestamp()) );

		if (!DatabaseConnectionEntityController::executePostgresQuery(m_pDbConnection, pQuery)) {
			LogError("Failed to execute Postgres query: " + pQuery->query() + " " + pQuery->errorMessage());
		}
		else if (pQuery->result().affected_rows() < 1) {
			LogError("Insert query failed to affect table.");
		}
		else {
			result = true;
		}


		return result;
	}
	bool DatabaseVersionDao::create(const std::vector<DatabaseVersionPocoPtr>& pPocos) const {
		bool result = false;

		uint64_t paramCounter = 0;
		std::string now = MiscUtility::getTimeISO(GeneralUtility::getTimestamp());

		auto pQueriesComp = m_pDbConnection->getPostgresQueriesComponent(true);
		std::string insertStatement = "INSERT INTO " + m_tableName + "(major, minor, patch, timestamp_created) VALUES ";

		PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();
		pQuery->query() = insertStatement;


		for (auto pPoco : pPocos) {
			std::string line = (0 == paramCounter) ? "": ", ";
			line += "($" + std::to_string(paramCounter+1) + ", $" + std::to_string(paramCounter+2) + ", $" + std::to_string(paramCounter+3) + ", $" + std::to_string(paramCounter+4) + ")";
			paramCounter += 4;
			pQuery->query() += line;

			pQuery->params().append(pPoco->major());
			pQuery->params().append(pPoco->minor());
			pQuery->params().append(pPoco->patch());
			pQuery->params().append(now);

			if (paramCounter > 65000) {
				pQuery->query() += ";";
				pQueriesComp->postgresQueries().push_back(pQuery);
				pQuery->clear();
				pQuery->query() = insertStatement;
				paramCounter = 0;
			}
		}

		if (pQuery->params().size() > 0) {
			pQuery->query() += ";";
			pQueriesComp->postgresQueries().push_back(pQuery);
		}

		if (!DatabaseConnectionEntityController::executePostgresQueries(m_pDbConnection)) {
			LogError("Failed to execute database queries for insert statement");
		}
		else {
			m_pDbConnection->removeComponent(PostgresQueriesComponent::identification);
			result = true;
		}


		return result;
	}

	

	DatabaseVersionPocoPtr DatabaseVersionDao::read(const int64_t id) const {
		DatabaseVersionPocoPtr pResult = nullptr;

		PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();
		pQuery->query() = "SELECT *, (EXTRACT(EPOCH FROM timestamp_created)*1000)::BIGINT as timestamp_created_raw FROM " + this->m_tableName + " WHERE version_id = $1;";
		pQuery->params().append(id);

		if (!DatabaseConnectionEntityController::executePostgresQuery(m_pDbConnection, pQuery)) {
			LogError("Failed to execute Postgres query: " + pQuery->query() + " " + pQuery->errorMessage());
		}
		else {
			auto resultSet = parseResultSet(pQuery->getResult());
			pResult = (resultSet.size() > 0) ? resultSet[0] : nullptr;
		}

		return pResult;
	}
	std::vector<DatabaseVersionPocoPtr> DatabaseVersionDao::readAll(int64_t limit) const {
		std::vector<DatabaseVersionPocoPtr> result;

		PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();
		pQuery->query() = "SELECT *, (EXTRACT(EPOCH FROM timestamp_created)*1000)::BIGINT as timestamp_created_raw FROM " + m_tableName;
		if (limit > 0) {
			pQuery->query() += " LIMIT $1";
			pQuery->params().append(limit);
		}
		
		if (!DatabaseConnectionEntityController::executePostgresQuery(m_pDbConnection, pQuery)) {
			LogError("Failed to execute Postgres query " + pQuery->query() + ": " + pQuery->errorMessage());
		}
		else {
			result = parseResultSet(pQuery->getResult());
		}

		return result;
	}

	bool DatabaseVersionDao::update(const DatabaseVersionPocoPtr& pPoco) const {
		if (nullptr == pPoco) throw NullpointerExcept("pPoco");
		bool result = false;
		if (0 >= pPoco->versionId()) {
			LogError("Version id " + std::to_string(pPoco->versionId()) + "of specified DatabaseVersionPoco is invalid.Can not update database entry.");
		}
		else {
			PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();
			pQuery->query() = "UPDATE " + this->m_tableName + " SET (major = $1, minor = $2, patch = $3) WHERE version_id = $4;";
			pQuery->params().append(pPoco->major());
			pQuery->params().append(pPoco->minor());
			pQuery->params().append(pPoco->patch());
			pQuery->params().append(pPoco->versionId());

			if (!DatabaseConnectionEntityController::executePostgresQuery(m_pDbConnection, pQuery)) {
				LogError("Failed to execute Postgres query " + pQuery->query() + " " + pQuery->errorMessage());
			}
			else {
				result = true;
			}
		}
		
		return result;
	}
	bool DatabaseVersionDao::update(const std::vector<DatabaseVersionPocoPtr>& pocos) const {
		bool result = false;
		if (pocos.size() != 0) {

			auto pQueries = m_pDbConnection->getPostgresQueriesComponent(true);
			auto pQuery = std::make_shared<PostgresQuery>();
			uint64_t parameterCount = 0;

			for (auto poco : pocos) {
				if (nullptr == poco) continue; // skip nullptr
				pQuery->query() += "UPDATE " + this->m_tableName + " SET (" + 
					"  major = $" + std::to_string(parameterCount + 1) +
					", minor = $" + std::to_string(parameterCount + 2) +
					", patch = $" + std::to_string(parameterCount + 3) + ") WHERE version_id = $" + std::to_string(parameterCount + 4) + ";\n";
				pQuery->params().append(poco->major());
				pQuery->params().append(poco->minor());
				pQuery->params().append(poco->patch());
				pQuery->params().append(poco->versionId());

				parameterCount += 4;
				if (parameterCount > 65000) {
					pQueries->postgresQueries().push_back(pQuery);
					pQuery = std::make_shared<PostgresQuery>();
					parameterCount = 0;
				}
			}
			if (!pQuery->query().empty()) pQueries->postgresQueries().push_back(pQuery);

			if (!DatabaseConnectionEntityController::executePostgresQueries(m_pDbConnection)) {
				LogError("Failed to execute Postgres queries for update statement.");
			}
			else {
				m_pDbConnection->removeComponent(PostgresQueriesComponent::identification);
				result = true;
			}
		}

		return result;
	}

	bool DatabaseVersionDao::erase(const int64_t id) const {
		bool result = false;
		if (0 >= id) {
			LogError("Database version id of " + std::to_string(id) + " is invalid. Can not delete this.");
		}
		else {
			PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();
			pQuery->query() = "DELETE FROM " + this->m_tableName + " WHERE version_id = $1;";
			pQuery->params().append(id);

			if (!DatabaseConnectionEntityController::executePostgresQuery(m_pDbConnection, pQuery)) {
				LogError("Failed to erase database version entry with id " + std::to_string(id) + ": " + pQuery->errorMessage());
			}
			else {
				result = true;
			}
		}
		
		return result;
	}
	bool DatabaseVersionDao::erase(const std::vector<int64_t>& ids) const {
		bool result = false;
		throw CrossForgeExcept("Not implemented yet, sorry.");

		return result;	
	}

	const int64_t DatabaseVersionDao::rowCount()const {
		if (nullptr == m_pDbConnection) throw NullpointerExcept("m_pdbConnection");
		int64_t result = -1;
		
		PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();
		pQuery->query() = "SELECT COUNT(version_id) FROM " + m_tableName + ";";

		if (!DatabaseConnectionEntityController::executePostgresQuery(m_pDbConnection, pQuery)) {
			LogError("Failed to execute Postgres query: " + pQuery->query());
		}
		else {
			result = pQuery->result()[0][0].as<int64_t>();
		}
		
		return result;
	}
}