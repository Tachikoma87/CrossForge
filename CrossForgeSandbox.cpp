#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#elif defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#else 
#endif


/******************************/
/***** INCLUDES ***************/
/******************************/
#include <stdio.h>
#include "Prototypes/applications/SCrossForgeTestApp.h"
#include "Prototypes/applications/SCrossForgeSimpleSceneApp.h"
#include "Prototypes/applications/SCrossForgeRestTestApp.h"

using namespace crossforge;

int main(int argc, char* argv[]) {

	CrossForgeSimpleSceneAppPtr pApp = CrossForgeSimpleSceneApp::instance();
	//CrossForgeTestAppPtr pApp = CrossForgeTestApp::instance();
	//CrossForgeRestTestAppPtr pApp = SCrossForgeRestTestApp::instance();
	try {
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