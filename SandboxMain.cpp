#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#elif defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#else 

#endif


#include "ExportLibrary.hpp"

#include "CForge/Core/SCrossForgeDevice.h"
#include "CForge/Core/SLogger.h"

#include <Examples/ExampleMinimumGraphicsSetup.hpp>
#include <Examples/ExampleMorphTargetAnimation.hpp>
#include <Examples/ExampleMultiViewport.hpp>
#include <Examples/ExampleSceneGraph.hpp>
#include <Examples/ExampleSkeletalAnimation.hpp>
#include <Examples/ExampleSkybox.hpp>
#include <Examples/ExampleSockets.hpp>


#include "Prototypes/TestScenes/ShadowTestScene.hpp"
#include "Prototypes/TestScenes/SkelAnimTestScene.hpp"
#include "Prototypes/TestScenes/IMUInputDeviceTestScene.hpp"
#include "Prototypes/TestScenes/SurfaceSamplerTestScene.hpp"
#include "Prototypes/TestScenes/ImageTestScene.hpp"
#include "Prototypes/TestScenes/ForwardTestScene.hpp"
#include "Prototypes/TestScenes/FrustumCullingTestScene.hpp"
#include "Prototypes/TestScenes/StickFigureTestScene.hpp"
#include "Prototypes/TestScenes/PrimitiveFactoryTestScene.hpp"
#include "Prototypes/TestScenes/EmscriptenGLTestScene.hpp"

#include "Prototypes/TestScenes/GUITestScene.hpp"

using namespace CForge;
using namespace Eigen;


ForwardTestScene*pScene = nullptr;

void mainLoop(void *pArg) {
	//auto* pScene = (ForwardTestScene*)pArg;
	pScene->mainLoop();
}

int main(int argc, char* argv[]) {
#ifdef WIN32
	_CrtMemState S1, S2, S3;
	_CrtMemCheckpoint(&S1);
#endif



	SCrossForgeDevice* pDev = nullptr;

	try {
		 pDev = SCrossForgeDevice::instance();
#if !defined(__EMSCRIPTEN__)
		 SLogger::logFile("Logs/ErrorLog.txt", SLogger::LOGTYPE_ERROR, true, true);
		 SLogger::logFile("Logs/DebugLog.txt", SLogger::LOGTYPE_DEBUG, true, true);
		 SLogger::logFile("Logs/InfoLog.txt", SLogger::LOGTYPE_INFO, true, true);
		 SLogger::logFile("Logs/WarningLog.txt", SLogger::LOGTYPE_WARNING, true, true);
#endif
	}
	catch (const CrossForgeException & e) {
		printf("Exception occurred during initialization. See Log.");
		SLogger::logException(e);
		if (nullptr != pDev) pDev->release();
		pDev = nullptr;
		char c;
		scanf("%c", &c);
		return -1;	
	}




	try {
		pScene = new ForwardTestScene();
		//ExampleSceneBase Scene;
		//ExampleMinimumGraphicsSetup Scene;
		//ExampleMorphTargetAnimation Scene;
		//ExampleMultiViewport Scene;
		//ExampleSceneGraph Scene;
		//ExampleSkeletalAnimation Scene;
		//ExampleSkybox Scene;
		//ExampleSocket Scene;
		
	
		//imuInputDeviceTestScene();	 
		//SkelAnimTestScene Scene;
		//shadowTest();	
		//surfaceSamplerTestScene();
		//vertexColorTestScene();
		//ForwardTestScene Scene;
		//PrimitiveFactoryTestScene Scene;


		//GUITestScene S;
		//S.guiTestScene();

		//imageTestScene();
	
		//FrustumCullingTestScene Scene;
		//StickFigureTestScene Scene;

		//Scene.init();
		pScene->init();

		printf("Scene initialized!\n");

#if defined(__EMSCRIPTEN__)
		emscripten_set_main_loop_arg(mainLoop, nullptr, 0, true);
#else
		//Scene.run();
		pScene->run();
#endif


		//exportLibrary();

	}
	catch (const CrossForgeException & e) {
		SLogger::logException(e);
		printf("Exception occurred. See Log.");
	}
	catch (...) {
		printf("A not handled exception occurred!\n");
	}


	if(nullptr != pDev) pDev->release();
	
	char c;
	//scanf("%c", &c);

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

	printf("QUtting now!");
	return 0;
}//main
