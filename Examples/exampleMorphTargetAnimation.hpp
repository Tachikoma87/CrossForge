/*****************************************************************************\
*                                                                           *
* File(s): exampleMorphTargetAnimation.hpp                                            *
*                                                                           *
* Content: Example scene that shows how to build, use, and visualize     *
*          a morph target animation.              *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_EXAMPLEMORPHTARGETANIMATION_HPP__
#define __CFORGE_EXAMPLEMORPHTARGETANIMATION_HPP__

#include "../CForge/AssetIO/SAssetIO.h"
#include "../CForge/Graphics/Shader/SShaderManager.h"
#include "../CForge/Graphics/STextureManager.h"

#include "../CForge/Graphics/GLWindow.h"
#include "../CForge/Graphics/GraphicsUtility.h"
#include "../CForge/Graphics/RenderDevice.h"

#include "../CForge/Graphics/Lights/DirectionalLight.h"
#include "../CForge/Graphics/Lights/PointLight.h"

#include "../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../CForge/Graphics/Actors/StaticActor.h"

#include "../CForge/Graphics/Actors/MorphTargetActor.h"
#include "../CForge/MeshProcessing/Builder/MorphTargetModelBuilder.h"

#include "SceneUtilities.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	void buildMTModel(T3DMesh<float>* pBaseMesh) {
		if (nullptr == pBaseMesh) throw NullpointerExcept("pBaseMesh");

		printf("Building morph target model...");
		uint64_t Start = CoreUtility::timestamp();

		// create morph target build and initialize with base mesh
		MorphTargetModelBuilder MTBuilder;
		MTBuilder.init(pBaseMesh);

		// define models we want to add
		// models have to bin in full vertex correspondence (same number of vertices, each having the same meaning)
		vector<pair<string, string>> MTList;
		MTList.push_back(pair("Anger",		"Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionAnger.obj"));
		MTList.push_back(pair("ChinRaised", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionChinRaised.obj"));
		MTList.push_back(pair("Disgust",	"Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionDisgust.obj"));
		MTList.push_back(pair("Fear",		"Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionFear.obj"));
		MTList.push_back(pair("Frown",		"Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionFrown.obj"));
		MTList.push_back(pair("Kiss",		"Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionKiss.obj"));
		MTList.push_back(pair("PuffCheeks", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionPuffCheeks.obj"));
		MTList.push_back(pair("Surprise",	"Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionSurprise.obj"));

		// load models and add to builder as targets
		T3DMesh<float> M;
		for (auto i : MTList) {
			M.clear();
			try {
				AssetIO::load(i.second, &M);
				MTBuilder.addTarget(&M, i.first);
			}
			catch (const CrossForgeException& e) {
				SLogger::logException(e);
			}
		}//for[all files]

		// build morph targets and retrieve them
		MTBuilder.build();
		MTBuilder.retrieveMorphTargets(pBaseMesh);

		printf(" finished int %d ms\n", uint32_t(CoreUtility::timestamp() - Start));
	}//buildMTModel

	void buildMTSequences(MorphTargetAnimationController* pController) {
		if (nullptr == pController) throw NullpointerExcept("pController");

		MorphTargetAnimationController::AnimationSequence Seq;

		// for every morph target we create a sequence
		// 1.25 seconds to activate the target, 0.5 seconds hold, and 1.25 seconds to go back to base mesh
		for (uint32_t i = 0; i < pController->morphTargetCount(); ++i) {
			Seq.clear();
			MorphTargetAnimationController::MorphTarget* pMT = pController->morphTarget(i);
			Seq.Name = pMT->Name;
			Seq.Targets.push_back(pMT->ID);
			Seq.Targets.push_back(pMT->ID);
			Seq.Targets.push_back(pMT->ID);
			Seq.Parameters.push_back(Vector3f(0.0f, 1.0f, 1.25f));
			Seq.Parameters.push_back(Vector3f(1.0f, 1.0f, 0.5f));
			Seq.Parameters.push_back(Vector3f(1.0f, 0.0f, 1.25f));
			pController->addAnimationSequence(&Seq);
		}//for[all morph targets

	}//buildMTSequences


	void exampleMorphTargetAnimation(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge - Minimum Graphics Setup";
		float FPS = 60.0f;

		bool const LowRes = false;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		if (LowRes) {
			WinWidth = 720;
			WinHeight = 576;
		}

		// create an OpenGL capable windows
		GLWindow RenderWin;
		RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), WindowTitle);

		// configure and initialize rendering pipeline
		RenderDevice RDev;
		RenderDevice::RenderDeviceConfig Config;
		Config.DirectionalLightsCount = 1;
		Config.PointLightsCount = 1;
		Config.SpotLightsCount = 0;
		Config.ExecuteLightingPass = true;
		Config.GBufferHeight = WinHeight;
		Config.GBufferWidth = WinWidth;
		Config.pAttachedWindow = &RenderWin;
		Config.PhysicallyBasedShading = true;
		Config.UseGBuffer = true;
		RDev.init(&Config);

		// configure and initialize shader configuration device
		ShaderCode::LightConfig LC;
		LC.DirLightCount = 1;
		LC.PointLightCount = 1;
		LC.SpotLightCount = 0;
		LC.PCFSize = 1;
		LC.ShadowBias = 0.0004f;
		LC.ShadowMapCount = 1;
		pSMan->configShader(LC);

		// initialize camera
		VirtualCamera Cam;
		Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		// initialize sun (key lights) and back ground light (fill light)
		Vector3f SunPos = Vector3f(-5.0f, 15.0f, 35.0f);
		Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
		DirectionalLight Sun;
		PointLight BGLight;
		Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
		// sun will cast shadows
		Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(10.0f, 10.0f, 0.1f, 1000.0f));
		BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.5f, Vector3f(0.0f, 0.0f, 0.0f));

		// set camera and lights
		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);
		RDev.addLight(&BGLight);

		// load skydome and a textured cube
		T3DMesh<float> M;	
		StaticActor Skydome;
		MorphTargetActor Face;

		SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		M.computePerVertexNormals();
		Skydome.init(&M);	
		M.clear();
		
		// load face model
		SAssetIO::load("Assets/ExampleScenes/FaceGenMale/MaleFace.obj", &M);
		SceneUtilities::setMeshShader(&M, 0.5f, 0.04f);
		M.computePerVertexNormals();
		// build the morph targets
		buildMTModel(&M);
		// initialize morph target controller and actor
		MorphTargetAnimationController MTController;
		MTController.init(&M);
		buildMTSequences(&MTController);
		Face.init(&M, &MTController);
		M.clear();

		// build scene graph
		SceneGraph SG;
		SGNTransformation RootSGN;
		RootSGN.init(nullptr);
		SG.init(&RootSGN);

		// add skydome
		SGNGeometry SkydomeSGN;
		SkydomeSGN.init(&RootSGN, &Skydome);
		SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

		// add cube
		SGNGeometry FaceSGN;
		SGNTransformation FaceTransformSGN;
		FaceTransformSGN.init(&RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
		FaceSGN.init(&FaceTransformSGN, &Face);
		FaceSGN.scale(Vector3f(0.01f, 0.01f, 0.01f));

		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;

		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SG.update(60.0f/FPS);

			// progres morph target animations
			MTController.update(FPS / 60.0f);

			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());
			Keyboard* pKeyboard = RenderWin.keyboard();

			// if one of key 1 through 5 is pressed, animation played
			// key 0 is wildcard playing a random animation

			int32_t PlayMTAnimation = -1;
			float MTAnimationSpeed = 1.0f;
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) MTAnimationSpeed = 3.0f;
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_CONTROL)) MTAnimationSpeed = 6.0f;

			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) PlayMTAnimation = 0;
			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) PlayMTAnimation = 1;
			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) PlayMTAnimation = 2;
			if (pKeyboard->keyPressed(Keyboard::KEY_4, true)) PlayMTAnimation = 3;
			if (pKeyboard->keyPressed(Keyboard::KEY_5, true)) PlayMTAnimation = 4;
			if (pKeyboard->keyPressed(Keyboard::KEY_6, true)) PlayMTAnimation = 5;
			if (pKeyboard->keyPressed(Keyboard::KEY_7, true)) PlayMTAnimation = 6;
			if (pKeyboard->keyPressed(Keyboard::KEY_8, true)) PlayMTAnimation = 7;
			if (pKeyboard->keyPressed(Keyboard::KEY_0, true)) PlayMTAnimation = CoreUtility::rand() % MTController.animationSequenceCount();
		

			if (-1 != PlayMTAnimation) {
				MorphTargetAnimationController::ActiveAnimation* pAnim = MTController.play(PlayMTAnimation, MTAnimationSpeed);
				Face.addAnimation(pAnim);
			}

			RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
			SG.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			SG.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RenderWin.swapBuffers();

			FPSCount++;
			if (CoreUtility::timestamp() - LastFPSPrint > 1000U) {
				char Buf[64];
				sprintf(Buf, "FPS: %d\n", FPSCount);
				FPS = float(FPSCount);
				FPSCount = 0;
				LastFPSPrint = CoreUtility::timestamp();

				RenderWin.title(WindowTitle + "[" + std::string(Buf) + "]");
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
		}//while[main loop]

		pSMan->release();

	}//exampleMinimumGraphicsSetup

}

#endif