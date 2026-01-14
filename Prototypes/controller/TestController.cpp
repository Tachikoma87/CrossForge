#include "TestController.h"

#include <drogon/drogon.h>

#include "../database/daos/DatabaseVersionDao.h"
#include <crossforge/database/provider/SDatabaseConnectionProvider.h>

using namespace drogon;

namespace crossforge {
	void TestController::getInfo(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr &)>&& callback, int id) const {
		Json::Value result;


		result["message"] = "Hello, CrossForge with Drogon - You requested an Id";
		result["id"] = id;

		result["data"] = Json::arrayValue;


		std::string level = req->getParameter("level");
		if (!level.empty()) {
			result["level"] = "You requested level " + level;
		}

		auto response = HttpResponse::newHttpJsonResponse(result);
		callback(response);
	}

	void TestController::quitApp(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)> &&callback) const {

		Json::Value result;

		result["message"] = "Ok, quitting app now";
		auto response = HttpResponse::newHttpJsonResponse(result);
		callback(response);

		drogon::app().quit();
	}

	TestController::TestController() {
		m_requestCounter = 0;
	}

	void TestController::readAllDatabaseVersionEntries(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)const {
		uint64_t start = GeneralUtility::getTimestamp();

		auto pConnection = DatabaseConnectionProvider::instance()->getDatabaseConnection("crossforgetest_peon");

		DatabaseVersionDaoPtr dbVersionDao = std::make_shared<DatabaseVersionDao>(pConnection);

		auto resultList = dbVersionDao->readAll(1000);

		Json::Value result = Json::arrayValue;

		for (auto pVersionPoco : resultList) result.append(pVersionPoco->toJson());

		auto response = HttpResponse::newHttpJsonResponse(result);
		response->addHeader("Access-Control-Allow-Origin", req.get()->headers().find("origin")->second);
		response->addHeader("Access-Control-Allow-Methods", "GET");
		callback(response);

		uint64_t responseTime = GeneralUtility::getTimestamp() - start;
		LogInfo("Response time for read all database version entries: " + std::to_string(responseTime) + " milliseconds.");
	}
}