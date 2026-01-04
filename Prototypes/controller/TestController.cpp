#include "TestController.h"

#include <drogon/drogon.h>

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
}