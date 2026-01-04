#include "DatabaseVersionDao.h"
#include "../../miscellaneous/MiscUtility.hpp"

namespace crossforge {

	DatabaseVersionDao::DatabaseVersionDao(DatabaseConnectionEntityPtr pDbConnection): DaoBase("version", pDbConnection) {

	}
	DatabaseVersionDao::~DatabaseVersionDao() {

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

		return result;
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
				pItem->timestampCreated() = MiscUtility::getTimestampFromIsoTime( r["timestamp_created"].as<std::string>() );
				//pItem->timestampCreated() = r["timestamp_created_raw"].as<int64_t>();
				result.push_back(pItem);
			}
			catch (std::exception& e) {
				LogError("Exception parsing result set: " + std::string(e.what()));
			}
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
			pResult = parseResultSet(pQuery->getResult())[0];
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
			LogError("Failed to execute Postgres query: " + pQuery->query() + " " + pQuery->errorMessage());
		}
		else {
			result = parseResultSet(pQuery->getResult());
		}

		return result;
	}

	bool DatabaseVersionDao::update(const DatabaseVersionPocoPtr& pPoco) const {
		bool result = false;

		return result;
	}
	bool DatabaseVersionDao::update(const std::vector<DatabaseVersionPocoPtr>& pPocos) const {
		bool result = false;

		return result;
	}

	bool DatabaseVersionDao::erase(const int64_t id) const {
		bool result = false;

		return result;
	}
	bool DatabaseVersionDao::erase(const std::vector<int64_t>& ids) const {
		bool result = false;

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