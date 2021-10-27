#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include "CForge/Core/SCrossForgeDevice.h"
#include "CForge/Core/SLogger.h"
#include "Terrain/TerrainSetup.hpp"

using namespace CForge;
using namespace Eigen;
using namespace Terrain;

int main() {
#ifdef WIN32
	_CrtMemState S1, S2, S3;
	_CrtMemCheckpoint(&S1);
#endif

	SCrossForgeDevice* crossForgeDevice = nullptr;

	try {
        crossForgeDevice = SCrossForgeDevice::instance();

		 SLogger::logFile("Logs/ErrorLog.txt", SLogger::LOGTYPE_ERROR, true, true);
		 SLogger::logFile("Logs/DebugLog.txt", SLogger::LOGTYPE_DEBUG, true, true);
		 SLogger::logFile("Logs/InfoLog.txt", SLogger::LOGTYPE_INFO, true, true);
		 SLogger::logFile("Logs/WarningLog.txt", SLogger::LOGTYPE_WARNING, true, true);
	}
	catch (const CrossForgeException & e) {
		SLogger::logException(e);
		printf("Exception occurred during init. See Log. ");

		if (crossForgeDevice != nullptr) crossForgeDevice->release();

		char c;
		scanf("%c", &c);
		return -1;
	}

	try {
        TerrainSetup();
	}
	catch (const CrossForgeException & e) {
		SLogger::logException(e);
		printf("Exception occurred. See Log.");
	}
	catch (...) {
		printf("An unhandled exception occurred!\n");
	}

	if (crossForgeDevice != nullptr) crossForgeDevice->release();
	
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
}