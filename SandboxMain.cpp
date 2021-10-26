//#ifdef __linux__
//#include <unistd.h>
//extern "C" {
//#include <linux/gpio.h>
//}
//#else
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//#endif
//
//// basic os imports
//#include <iostream>
//#include <cstdio>
//#include <inttypes.h>
//#include <memory.h>
//#include <thread>
//#include <chrono>
//
//// Glad and GLFW3
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include "DevicesTestFunctions.hpp"
//
//#include "CForge/Core/SLogger.h"
//#include "CForge/Core/SCrossForgeDevice.h"
//
//#include "CForge/Graphics/STextureManager.h"
//#include "CForge/Graphics/Shader/SShaderManager.h"
//#include "CForge/Graphics/SceneGraph/SceneGraph.h"
//#include "CForge/Graphics/SceneGraph/SGNGeometry.h"
//#include "CForge/Graphics/SceneGraph/SGNTransformation.h"
//#include "CForge/Graphics/Actors/StaticActor.h"
//#include "CForge/Graphics/Lights/DirectionalLight.h"
//#include "CForge/Graphics/Lights/PointLight.h"
//#include "CForge/Graphics/Lights/SpotLight.h"
//#include "CForge/AssetIO/SAssetIO.h"
//#include "CForge/Graphics/GraphicsUtility.h"
//
////#include "Prototypes/Shader/ShaderCode.h"
//#include "Prototypes/Actor/SkeletalActor.h"
//#include "Prototypes/Actor/SkeletalAnimationController.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#else 

#endif

#include "CForge/Core/SCrossForgeDevice.h"
#include "CForge/Core/SLogger.h"


#include "Examples/MinimumGraphicalSetup.hpp"

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
		MinimumGraphicalSetup();
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