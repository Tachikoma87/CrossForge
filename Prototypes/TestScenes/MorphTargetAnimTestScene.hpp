/*****************************************************************************\
*                                                                           *
* File(s): MorphTargetAnimTestScene.hpp                                            *
*                                                                           *
* Content:    *
*                                                  *
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
#ifndef __CFORGE_MORPHTARGETANIMTESTSCENE_HPP__
#define __CFORGE_MORPHTARGETANIMTESTSCENE_HPP__

#include <glad/glad.h>

#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"
#include "../../CForge/Graphics/STextureManager.h"

#include "../../CForge/Graphics/GLWindow.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Graphics/RenderDevice.h"

#include "../../CForge/Graphics/Lights/DirectionalLight.h"

#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../../CForge/Graphics/Actors/StaticActor.h"

#include "../../Examples/SceneUtilities.hpp"

#include "../Actor/MorphTargetActor.h"
#include "../Actor/MorphTargetAnimationController.h"
#include "../Builder/MorphTargetModelBuilder.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	void buildMTModel(T3DMesh<float>* pBaseMesh) {
		if (nullptr == pBaseMesh) throw NullpointerExcept("pBaseMesh");
		MorphTargetModelBuilder MTBuilder;
		MTBuilder.init(pBaseMesh);
		

		vector<pair<string, string>> MTList;
		MTList.push_back(pair("Anger",			"Assets/FGMaleModel/MaleFace_ExpressionAnger.obj"));
		MTList.push_back(pair("Baring Teeth",	"Assets/FGMaleModel/MaleFace_ExpressionBaringTeeth.obj"));
		MTList.push_back(pair("Chin Raised",	"Assets/FGMaleModel/MaleFace_ExpressionChinRaised.obj"));
		MTList.push_back(pair("Disgust",		"Assets/FGMaleModel/MaleFace_ExpressionDisgust.obj"));
		MTList.push_back(pair("Fear",			"Assets/FGMaleModel/MaleFace_ExpressionFear.obj"));
		
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

		MTBuilder.build();
		MTBuilder.retrieveMorphTargets(pBaseMesh);

		printf("Finished builder morph target model.\n");
	}//buildMTModel

	void morphTargetAnimTestScene(void) {
		std::string Title = "Morph Target Animation";

		SAssetIO* pAssIO = SAssetIO::instance();
		STextureManager* pTexMan = STextureManager::instance();
		SShaderManager* pSMan = SShaderManager::instance();

		bool const LowRes = false;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		if (LowRes) {
			WinWidth = 720;
			WinHeight = 576;
		}

		GLWindow RenderWin;
		RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), "Absolute Minimum Setup");
		gladLoadGL();

		std::string GLError;
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		RenderDevice RDev;
		RenderDevice::RenderDeviceConfig Config;
		Config.DirectionalLightsCount = 1;
		Config.PointLightsCount = 0;
		Config.SpotLightsCount = 0;
		Config.ExecuteLightingPass = true;
		Config.GBufferHeight = WinHeight;
		Config.GBufferWidth = WinWidth;
		Config.pAttachedWindow = &RenderWin;
		Config.PhysicallyBasedShading = true;
		Config.UseGBuffer = true;
		RDev.init(&Config);

		ShaderCode::LightConfig LC;
		LC.DirLightCount = 1;
		LC.PCFSize = 1;
		LC.PointLightCount = 0;
		LC.ShadowBias = 0.00004f;
		LC.ShadowMapCount = 1;
		LC.SpotLightCount = 0;
		pSMan->configShader(LC);


		VirtualCamera Cam;
		Cam.init(Vector3f(0.0f, 0.0f, 5.0f), Vector3f::UnitY());
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);
		Cam.position(Vector3f(0.0f, 2.0f, 10.0f));

		Vector3f SunPos = Vector3f(5.0f, 25.0f, 25.0f);
		DirectionalLight Sun;
		Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
		//Sun.initShadowCasting(2048, 2048, GraphicsUtility::orthographicProjection(50.0f, 50.0f, 0.1f, 2000.0f));

		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);

		SceneGraph SGTest;

		StaticActor Ground;
		SGNTransformation GroundTransformSGN;
		SGNGeometry GroundSGN;

		SGNGeometry CubeSGN;
		SGNTransformation CubeTransformSGN;
		StaticActor Cube;

		MorphTargetActor Face;
		SGNGeometry FaceSGN;
		SGNTransformation FaceTransformSGN;

		T3DMesh<float> M;

		SAssetIO::load("Assets/FGMaleModel/MaleFace.obj", &M);
		SceneUtilities::setMeshShader(&M, 0.5f, 0.04f);
		M.computePerVertexNormals();
		buildMTModel(&M);

		// create morph target animation controller for this asset
		MorphTargetAnimationController MTAnimController;
		MTAnimController.init(&M);

		Face.init(&M, &MTAnimController);
		FaceTransformSGN.init(nullptr);
		const float FaceScale = 0.01f;
		FaceSGN.init(&FaceTransformSGN, &Face, Vector3f(0.0f, 3.0f, 0.0f), Quaternionf::Identity(), Vector3f(FaceScale, FaceScale, FaceScale));
		M.clear();

		SAssetIO::load("Assets/TexturedCube.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.1f, 0.04f);
		M.computePerVertexNormals();
		Cube.init(&M);
		CubeTransformSGN.init(nullptr);
		CubeSGN.init(&CubeTransformSGN, &Cube, Vector3f(0.0f, 2.0f, 0.0f));
		CubeSGN.enable(false, false);

		SAssetIO::load("Assets/TexturedGround.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.1f, 0.04f);
		//M.computePerVertexNormals();
		Ground.init(&M);
		GroundTransformSGN.init(nullptr);
		Quaternionf R;
		R = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());
		GroundSGN.init(&GroundTransformSGN, &Ground, Eigen::Vector3f::Zero(), R, Eigen::Vector3f(300.0f, 300.0f, 300.0f));

		SGTest.init(&GroundTransformSGN);
		GroundTransformSGN.addChild(&CubeTransformSGN);
		GroundTransformSGN.addChild(&FaceTransformSGN);

		// rotate about the y-axis at 45 degree every second
		R = AngleAxisf(GraphicsUtility::degToRad(45.0f / 60.0f), Vector3f::UnitY());
		CubeTransformSGN.rotationDelta(R);

		int64_t LastFPSPrint = GetTickCount();
		int32_t FPSCount = 0;

		Keyboard* pKeyboard = RenderWin.keyboard();
		Mouse* pMouse = RenderWin.mouse();

		// create morph target animation sequence
		MorphTargetAnimationController::AnimationSequence Seq;
		Seq.Name = "Anger";
		Seq.Targets.push_back(0);
		Seq.Parameters.push_back(Vector3f(0.0f, 1.0f, 2.0f));
		Seq.Targets.push_back(0);
		Seq.Parameters.push_back(Vector3f(1.0f, 0.0f, 2.0f));
		MTAnimController.addAnimationSequence(&Seq);

		Seq.clear();
		Seq.Name = "Baring Teeth";
		Seq.Targets.push_back(1);
		Seq.Parameters.push_back(Vector3f(0.0f, 1.0f, 1.0f));
		Seq.Targets.push_back(1);
		Seq.Parameters.push_back(Vector3f(1.0f, 0.0f, 1.0f));
		MTAnimController.addAnimationSequence(&Seq);

		Seq.clear();
		Seq.Name = "Chin Raiser";
		Seq.Targets.push_back(2);
		Seq.Parameters.push_back(Vector3f(0.0f, 1.0f, 1.0f));
		Seq.Targets.push_back(2);
		Seq.Parameters.push_back(Vector3f(1.0f, 0.0f, 1.0f));
		MTAnimController.addAnimationSequence(&Seq);

		Seq.clear();
		Seq.Name = "Disgust";
		Seq.Targets.push_back(3);
		Seq.Parameters.push_back(Vector3f(0.0f, 1.0f, 1.0f));
		Seq.Targets.push_back(3);
		Seq.Parameters.push_back(Vector3f(1.0f, 1.0f, 0.5f));
		Seq.Targets.push_back(3);
		Seq.Parameters.push_back(Vector3f(1.0f, 0.0f, 1.0f));
		MTAnimController.addAnimationSequence(&Seq);

		Seq.clear();
		Seq.Name = "Fear";
		Seq.Targets.push_back(4);
		Seq.Parameters.push_back(Vector3f(0.0f, 1.0f, 1.0f));
		Seq.Targets.push_back(4);
		Seq.Parameters.push_back(Vector3f(1.0f, 1.0f, 2.0f));
		Seq.Targets.push_back(4);
		Seq.Parameters.push_back(Vector3f(1.0f, 0.0f, 1.0f));
		MTAnimController.addAnimationSequence(&Seq);

		
		while (!RenderWin.shutdown()) {
			RenderWin.update();
			MTAnimController.update(1.0f);

			SceneUtilities::defaultCameraUpdate(&Cam, pKeyboard, pMouse);

			float MTAnimSpeed = 1.0f;
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) MTAnimSpeed = 2.0f;

			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) {
				MorphTargetAnimationController::ActiveAnimation* pAnim = MTAnimController.play(0, MTAnimSpeed);
				Face.addAnimation(pAnim);
			}
			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) {
				MorphTargetAnimationController::ActiveAnimation* pAnim = MTAnimController.play(1, MTAnimSpeed);
				Face.addAnimation(pAnim);
			}
			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) {
				MorphTargetAnimationController::ActiveAnimation* pAnim = MTAnimController.play(2, MTAnimSpeed);
				Face.addAnimation(pAnim);
			}
			if (pKeyboard->keyPressed(Keyboard::KEY_4, true)) {
				MorphTargetAnimationController::ActiveAnimation* pAnim = MTAnimController.play(3, MTAnimSpeed);
				Face.addAnimation(pAnim);
			}
			if (pKeyboard->keyPressed(Keyboard::KEY_5, true)) {
				MorphTargetAnimationController::ActiveAnimation* pAnim = MTAnimController.play(4, MTAnimSpeed);
				Face.addAnimation(pAnim);
			}

			SGTest.update(1.0f);

			RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
			SGTest.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			SGTest.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RenderWin.swapBuffers();

			FPSCount++;
			if (GetTickCount() - LastFPSPrint > 1000) {
				char Status[128];
				sprintf(Status, "FPS: %d\n", FPSCount);
				FPSCount = 0;
				LastFPSPrint = GetTickCount();
				RenderWin.title(Title + "[" + std::string(Status) + "]");
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
		}//while[main loop]


		pAssIO->release();
		pTexMan->release();
		pSMan->release();
	}//morphTargetAnimTestScene


}//name space


#endif 