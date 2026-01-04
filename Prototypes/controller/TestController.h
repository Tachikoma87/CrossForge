/*****************************************************************************\
*                                                                           *
* File(s): TestController.h and TestController.cpp                       *
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
#ifndef __CROSSFORGE_TESTCONTROLLER_H__
#define __CROSSFORGE_TESTCONTROLLER_H__

#include <drogon/HttpController.h>

namespace crossforge {
	class TestController: public drogon::HttpController<TestController> {
	public:
		METHOD_LIST_BEGIN
		METHOD_ADD(TestController::getInfo, "/{id}", drogon::Get);
		METHOD_ADD(TestController::quitApp, "/quit", drogon::Get);
		METHOD_LIST_END

		void getInfo(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)> &&callback, int id) const;
		
		void quitApp(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)> &&callback)const;

		TestController();
	protected:
		int64_t m_requestCounter;
	};
}

#endif 
