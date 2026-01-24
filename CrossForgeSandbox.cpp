/*****************************************************************************\
*                                                                           *
* File(s): CrossForgeSandbox.cpp                       *
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


/******************************/
/***** INCLUDES ***************/
/******************************/
#include <stdio.h>
#include "Prototypes/applications/SCrossForgeTestApp.h"
#include "Prototypes/applications/SCrossForgeSimpleSceneApp.h"
#include "Prototypes/applications/SCrossForgeRestTestApp.h"
#include "Prototypes/examples/BasicWindowSetup.h"

using namespace crossforge;

using AppType = BasicWindowSetup;
//using AppType = CrossForgeSimpleSceneApp;
//using AppType = CrossForgeRestTestApp;


std::shared_ptr<AppType> pApp = nullptr;  

int main(int argc, char* argv[]) {

	try {
		LogInfo("Starting CrossForge Sandbox Application now...");
		pApp = std::make_shared<AppType>();
		pApp->initialize();
		pApp->start();	
	}
	catch (CrossForgeException& e) {
		printf("Something went wrong with crossforge: %s\n", e.getMessage().c_str());
	}
	catch (std::exception& e) {
		printf("Something unexpected happened: %s\n", e.what());
	}
	
	return 0;
}