/*****************************************************************************\
*                                                                           *
* File(s): TestRouter.h and TestRouter.cpp                       *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_TESTROUTER_H__
#define __CROSSFORGE_TESTROUTER_H__

#include <drogon/HttpController.h>

namespace crossforge {
	class TestRouter: public drogon::HttpController<TestRouter> {
	public:
		METHOD_LIST_BEGIN
		METHOD_ADD(TestRouter::getInfo, "/{id}", drogon::Get);
		ADD_METHOD_TO(TestRouter::getInfo, "/{id}", drogon::Get);
		METHOD_ADD(TestRouter::quitApp, "/quit", drogon::Get);
		ADD_METHOD_TO(TestRouter::readAllDatabaseVersionEntries, "/read/databaseVersion/all", drogon::Get);
		METHOD_LIST_END

		void getInfo(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)> &&callback, int id) const;
		
		void quitApp(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)> &&callback)const;

		void readAllDatabaseVersionEntries(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)const;

		TestRouter();
	protected:
		int64_t m_requestCounter;
	};
}

#endif 
