#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#else 

#endif

#include "CForge/Core/SCrossForgeDevice.h"
#include "CForge/Core/SLogger.h"

#include "Examples/MinimumGraphicalSetup.hpp"
#include "Prototypes/TestScenes/ShadowTestScene.hpp"
#include "Prototypes/TestScenes/MorphTargetAnimTestScene.hpp"
#include "Prototypes/TestScenes/LODTestScene.hpp"

#include <glad/glad.h>

using namespace CForge;
using namespace Eigen;

int main(int argc, char* argv[]) {
#ifdef WIN32
	_CrtMemState S1, S2, S3;
	_CrtMemCheckpoint(&S1);
#endif

	SCrossForgeDevice* pDev = nullptr;

	try {
		 pDev = SCrossForgeDevice::instance();
		 // to fetch OpenGL function points in main program

		 SLogger::logFile("Logs/ErrorLog.txt", SLogger::LOGTYPE_ERROR, true, true);
		 SLogger::logFile("Logs/DebugLog.txt", SLogger::LOGTYPE_DEBUG, true, true);
		 SLogger::logFile("Logs/InfoLog.txt", SLogger::LOGTYPE_INFO, true, true);
		 SLogger::logFile("Logs/WarningLog.txt", SLogger::LOGTYPE_WARNING, true, true);
	}
	catch (const CrossForgeException & e) {
		SLogger::logException(e);
		printf("Exception occurred during init. See Log.");
		if (nullptr != pDev) pDev->release();
		pDev = nullptr;
		char c;
		scanf("%c", &c);
		return -1;	
	}

	try {
		LODTestScene();
		//MinimumGraphicalSetup();
		//shadowTest();
		//morphTargetAnimTestScene();
	}
	catch (const CrossForgeException & e) {
		SLogger::logException(e);
		printf("Exception occurred. See Log.");
	}
	catch (...) {
		printf("A not handled exception occurred!\n");
	}


	if(nullptr != pDev) pDev->release();
	
#ifdef WIN32
	// dump memory statics
	_CrtMemCheckpoint(&S2);
	_CrtMemDifference(&S3, &S1, &S2);
	_CrtMemDumpStatistics(&S3);

	// disable memleak dump
	int Tmp = _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
	Tmp &= ~_CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(Tmp);
#endif
}//main