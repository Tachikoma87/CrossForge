#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#else 

#endif

#include "CForge/Core/SCrossForgeDevice.h"
#include "CForge/Core/SLogger.h"

#include "Examples/exampleSceneBase.hpp"
#include "Examples/exampleMinimumGraphicsSetup.hpp"
#include "Examples/exampleSkeletalAnimation.hpp"
#include "Examples/exampleSkeletalAnimationThrice.hpp"
#include "Examples/exampleSkeletalCollisionResolver.hpp"
#include "Examples/exampleMorphTargetAnimation.hpp"
#include "Examples/exampleSkybox.hpp"
#include "Examples/exampleMultiViewport.hpp"
#include "Examples/exampleSockets.hpp"
#include "Examples/exampleShapeDeformer.hpp"
#include "Examples/exportAngles.hpp"

#include "Prototypes/TestScenes/ShadowTestScene.hpp"
#include "Prototypes/TestScenes/VertexColorTestScene.hpp"
#include "Prototypes/TestScenes/ForestTestScene.hpp"
#include "Prototypes/TestScenes/SkelAnimTestScene.hpp"
#include "Prototypes/TestScenes/IMUInputDeviceTestScene.hpp"
#include "Prototypes/TestScenes/SurfaceSamplerTestScene.hpp"
#include "Prototypes/TestScenes/ShapeDeformerTestScene.hpp"

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
		//exampleSceneBase();
		//exampleMinimumGraphicsSetup();
		//exampleMorphTargetAnimation();
		//exampleMultiViewport();
	//	exampleSkeletalAnimation(); 
		
		//exampleSkeletalAnimationThrice(); 
		//exportAngles();
		//exampleSkeletonCollisionResolver();
		//exampleSkybox();
		//exampleSockets();
	
		//forestTestScene();	
		//imuInputDeviceTestScene();	 
		//skelAnimTestScene();
		//shadowTest();	
//		vertexColorTestScene();

		int32_t startFrame = 0;
		int32_t endFrame = 3;
		int32_t method = 3;
		//int32_t method = 1;
		//int32_t method = 2;
		
		/*if (!File::exists("FBXsamplePoints1000.txt")) {
			surfaceSamplerTestScene();
		}*/

		//if (!File::exists("MuscleMan" + to_string(startFrame) + ".obj")) {
			uint64_t Start = CoreUtility::timestamp();
			exampleShapeDeformer(method, startFrame, endFrame);
			printf("ShapeDeformation finished in %d ms\n", uint32_t(CoreUtility::timestamp() - Start));
		//}
		shapeDeformerTest(startFrame, endFrame);
		
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
}//main