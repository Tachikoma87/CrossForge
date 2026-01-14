#define NOMINMAX
#include "SCrossForgeRestTestApp.h"
#include <thread>

#include <drogon/drogon.h>
#include "../miscellaneous/MiscUtility.hpp"
#include <crossforge/math/CrossForgeMath.h>
#include "../database/daos/DatabaseVersionDao.h"

namespace crossforge {

	std::shared_ptr<SCrossForgeRestTestApp> SCrossForgeRestTestApp::m_pInstance = nullptr;

	std::shared_ptr<SCrossForgeRestTestApp> SCrossForgeRestTestApp::instance() {
		if (nullptr == m_pInstance) {
			SCrossForgeRestTestApp* pInstance = new SCrossForgeRestTestApp();
			m_pInstance = std::make_shared<SCrossForgeRestTestApp>(*pInstance);
		}
		return m_pInstance;
	}

	void SCrossForgeRestTestApp::destroy() {
		m_pInstance = nullptr;
	}


	SCrossForgeRestTestApp::SCrossForgeRestTestApp() {
		m_sleepInterval = 50;
	}
	SCrossForgeRestTestApp::~SCrossForgeRestTestApp() {

	}


	void SCrossForgeRestTestApp::initialize() {
		// create entity
		m_pDbConnection = std::make_shared<DatabaseConnectionEntity>();
		auto pDbSettingsComp = m_pDbConnection->getDatabaseConnectionSettingsComponent(true);

		// set connection information
		pDbSettingsComp->setDatabaseName("crossforgetest");
		pDbSettingsComp->setHostAddress("127.0.0.1");
		pDbSettingsComp->setPort(15600);
		pDbSettingsComp->setUserName("crossforge-peon");
		pDbSettingsComp->setUserPassword("SecretCr0ssF0rge.pw");

		m_pDbConnectionProvider = DatabaseConnectionProvider::instance();
		m_pDbConnectionProvider->registerDatabaseConnection("crossforgetest_peon", m_pDbConnection);
		

		uint64_t timestampsStart = GeneralUtility::getTimestamp();

		if (true) {
			uint64_t start = GeneralUtility::getTimestamp();
			for (int i = 0; i < 1; ++i) {
				databaseDaoTest();
			}
			uint64_t runtime = GeneralUtility::getTimestamp() - start;
			LogInfo("Runtime for database dao test was " + std::to_string(runtime / 1000.0f) + " seconds.");

		}
		else {
			DatabaseConnectionEntityController::openPostgresConnection(m_pDbConnection);
			std::vector<DatabaseConnectionEntityPtr> connectionPool;
			for (int32_t run = 0; run < 4; ++run) {
				LogInfo("Starting test run " + std::to_string(run) + " now.");
				connectionPool.push_back(m_pDbConnectionProvider->getDatabaseConnection("crossforgetest_peon"));

				std::thread threads[m_threadCount];
				uint64_t executionTimes[m_threadCount];
				for (int32_t i = 0; i < m_threadCount; ++i) {
					threads[i] = std::thread(&SCrossForgeRestTestApp::databaseMultiThreadTestFunc, this, i);
				}
				uint64_t totalExecutionTimeWrite = 0;
				uint64_t totalExecutionTimeRead = 0;
				uint64_t totalExecutionTimeWriteBatch = 0;
				uint64_t totalExecutionTimeReadBatch = 0;
				for (int32_t i = 0; i < m_threadCount; ++i) {
					threads[i].join();
					totalExecutionTimeWrite += m_executionTimesWrite[i];
					totalExecutionTimeRead += m_executionTimesRead[i];
					totalExecutionTimeWriteBatch += m_executionTimesWriteBatch[i];
					totalExecutionTimeReadBatch += m_executionTimesReadBatch[i];
				}
				uint64_t runtime = GeneralUtility::getTimestamp() - timestampsStart;
				std::string msg = "Creating 1000 db entries took " + std::to_string(runtime / 1000.0f) + " seconds.";
				LogInfo(msg);
				msg = "Average execution time write was " + std::to_string(totalExecutionTimeWrite / m_threadCount / 1000.0f) + " seconds";
				LogInfo(msg);
				msg = "Average execution time read was " + std::to_string(totalExecutionTimeRead / m_threadCount / 1000.0f) + " seconds";
				LogInfo(msg);

				msg = "Average execution time write batch was " + std::to_string(totalExecutionTimeWriteBatch / m_threadCount / 1000.0f) + " seconds";
				LogInfo(msg);
				msg = "Average execution time read batch was " + std::to_string(totalExecutionTimeReadBatch / m_threadCount / 1000.0f) + " seconds";
				LogInfo(msg);
			}
		}

		drogon::app().setLogPath("./")
			.setLogLevel(trantor::Logger::kWarn)
			.addListener("0.0.0.0", 15700)
			.setThreadNum(4)
			.run();
	}

	void SCrossForgeRestTestApp::databaseMultiThreadTestFunc(int64_t index) {
		auto pDbConnection = m_pDbConnectionProvider->getDatabaseConnection("crossforgetest_peon");

		auto pQueriesComp = pDbConnection->getPostgresQueriesComponent(true);

		int64_t minorMax = 10;
		int64_t patchMax = 50;
		
		if (nullptr == pDbConnection || !DatabaseConnectionEntityController::isPostgresConnectionOpen(pDbConnection)) {
			LogError("Thread " + std::to_string(index) + ": Unable to get database connection.");
		}
		else {
			LogInfo("Thread " + std::to_string(index) + " starting work now.");

			// single execution test
			uint64_t start = GeneralUtility::getTimestamp();
			for (int64_t minor = 0; minor < minorMax; ++minor) {
				for (int64_t patch = 0; patch < patchMax; ++patch) {
					databaseWriteTest(pDbConnection, index, minor, patch, pQueriesComp);
				}
			}
			m_executionTimesWrite[index] = GeneralUtility::getTimestamp() - start;

			// batch test
			start = GeneralUtility::getTimestamp();
			DatabaseConnectionEntityController::executePostgresQueries(pDbConnection);
			m_executionTimesWriteBatch[index] = GeneralUtility::getTimestamp() - start;
			pQueriesComp->clear();

			// read single execution test
			start = GeneralUtility::getTimestamp();
			for (int64_t minor = 0; minor < minorMax; ++minor) {
				for (int64_t patch = 0; patch < patchMax; ++patch) {
					databaseReadTest(pDbConnection, index, minor, patch, pQueriesComp);
				}
			}
			m_executionTimesRead[index] = GeneralUtility::getTimestamp() - start;

			// batch read test
			start = GeneralUtility::getTimestamp();
			DatabaseConnectionEntityController::executePostgresQueries(pDbConnection);
			m_executionTimesReadBatch[index] = GeneralUtility::getTimestamp() - start;
		}

		LogInfo("Thread " + std::to_string(index) + " finished work.");
		
	}

	bool SCrossForgeRestTestApp::databaseDaoTest() {
		DatabaseConnectionEntityPtr pDbConnection = m_pDbConnectionProvider->getDatabaseConnection("crossforgetest_peon");

		DatabaseVersionPocoPtr pDatabaseVersionPoco = std::make_shared<DatabaseVersionPoco>();

		DatabaseVersionDao databaseVersionDao(pDbConnection);

		int64_t majorMax = 5;
		int64_t minorMax = 5;
		int64_t patchMax = 5;

		if (true) {
			std::vector<DatabaseVersionPocoPtr> pocoList;
			for (int64_t major = 0; major < majorMax; major++) {
				for (int64_t minor = 0; minor < minorMax; minor++) {
					for (int64_t patch = 0; patch < patchMax; patch++) {
						DatabaseVersionPocoPtr pPoco = std::make_shared<DatabaseVersionPoco>();
						pPoco->major() = major;
						pPoco->minor() = minor;
						pPoco->patch() = patch;
						pocoList.push_back(pPoco);
					}
				}
			}

			if (!databaseVersionDao.create(pocoList)) LogError("Failed to create database version with poco list");
			else LogInfo("Successfully created " + std::to_string(pocoList.size()) + " database version entries with batched statement.");
		}
		else {
			auto pQueriesComp = pDbConnection->getPostgresQueriesComponent(true);
			std::string now = MiscUtility::getTimeISO(GeneralUtility::getTimestamp());
			for (int64_t major = 0; major < majorMax; major++) {
				for (int64_t minor = 0; minor < minorMax; minor++) {
					for (int64_t patch = 0; patch < patchMax; patch++) {
						PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();
						pQuery->query() = "INSERT INTO version (major, minor, patch, timestamp_created) VALUES ($1, $2, $3, $4);";
						pQuery->params().append(major);
						pQuery->params().append(minor);
						pQuery->params().append(patch);
						pQuery->params().append(now);
						pQueriesComp->postgresQueries().push_back(pQuery);
					}
				}
			}

			if (!DatabaseConnectionEntityController::executePostgresQueries(pDbConnection)) LogError("Failed to execute queries.");
			else LogInfo("Successfully executed batched queries.");
		}


		//auto set = databaseVersionDao.readAll();
			//LogInfo("Retrieved rows: " + std::to_string(set.size()));
			/*int64_t rows = databaseVersionDao.rowCount();
			LogInfo("DatabaseVersion table has " + std::to_string(rows) + " entries.");*/

		return true;
	}

	bool SCrossForgeRestTestApp::databaseWriteTest(DatabaseConnectionEntityPtr pDbConnection, int64_t major, int64_t minor, int64_t patch, PostgresQueriesComponentPtr pBatchComponent) {
		bool result = false;

		PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();

		pQuery->query() = "INSERT INTO version (major, minor, patch, timestamp_created) VALUES ($1, $2, $3, $4);";
		pQuery->params().append(major);
		pQuery->params().append(minor);
		pQuery->params().append(patch);
		pQuery->params().append(MiscUtility::getTimeISO(GeneralUtility::getTimestamp()));

		/*if (!DatabaseConnectionEntityController::executePostgresQuery(pDbConnection, pQuery)) LogError("Failed to execute query.");
		else result = true;*/

		if (nullptr != pBatchComponent) pBatchComponent->postgresQueries().push_back(pQuery);
		result = true;

		return result;
	}

	bool SCrossForgeRestTestApp::databaseReadTest(DatabaseConnectionEntityPtr pDbConnection, int64_t major, int64_t minor, int64_t patch, PostgresQueriesComponentPtr pBatchComponent) {
		bool result = false;

		PostgresQueryPtr pQuery = std::make_shared<PostgresQuery>();
		pQuery->query() = "SELECT * FROM version WHERE major = $1 AND minor = $2 AND patch = $3;";
	/*	pQuery->params().append(major);
		pQuery->params().append(minor);
		pQuery->params().append(patch);*/


		pQuery->query() = "SELECT * FROM version WHERE version_id = $1;";
		pQuery->params().append(CrossForgeMath::randRange<int64_t>(0L, 20000L));

		/*if (!DatabaseConnectionEntityController::executePostgresQuery(pDbConnection, pQuery)) LogError("Failed to execute query.");
		else result = true;

		for (auto r : pQuery->result()) {
			int64_t ma = r["major"].as<int64_t>();
			int64_t mi = r["minor"].as<int64_t>();
		}*/

		if (nullptr != pBatchComponent) pBatchComponent->postgresQueries().push_back(pQuery);
		result = true;
		return result;
	}

	void SCrossForgeRestTestApp::update() {


		this->stop();
	}

}

	