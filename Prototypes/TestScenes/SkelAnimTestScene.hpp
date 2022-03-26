/*****************************************************************************\
*                                                                           *
* File(s): exampleSkeletalAnimation.hpp                                            *
*                                                                           *
* Content: Example scene that shows how to use skeletal animation.          *
*                       *
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
#ifndef __CFORGE_SKELANIMTESTSCENE_HPP__
#define __CFORGE_SKELANIMTESTSCENE_HPP__

#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"
#include "../../CForge/Graphics/STextureManager.h"

#include "../../CForge/Graphics/GLWindow.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Graphics/RenderDevice.h"

#include "../../CForge/Graphics/Lights/DirectionalLight.h"
#include "../../CForge/Graphics/Lights/PointLight.h"

#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../../CForge/Graphics/Actors/StaticActor.h"
#include "../../CForge/Graphics/Actors/SkeletalActor.h"

#include "../../Examples/SceneUtilities.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	void skelAnimTestScene(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge - Skeletal Animation Example";
		float FPS = 60.0f;

		bool const LowRes = false;
		bool const HighRes = false;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		if (LowRes) {
			WinWidth = 720;
			WinHeight = 576;
		}

		if (HighRes) {
			WinWidth = 1920;
			WinHeight = 1080;
		}

		// create an OpenGL capable windows
		GLWindow RenderWin;
		RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), WindowTitle);

		// configure and initialize rendering pipeline
		RenderDevice RDev;
		RenderDevice::RenderDeviceConfig Config;
		Config.DirectionalLightsCount = 2;
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
		LC.DirLightCount = 2;
		LC.PointLightCount = 1;
		LC.SpotLightCount = 0;
		LC.PCFSize = 0;
		LC.ShadowBias = 0.00001f;
		LC.ShadowMapCount = 1;
		pSMan->configShader(LC);

		// initialize camera
		VirtualCamera Cam;
		Cam.init(Vector3f(15.0f, 5.0f, 35.0f), Vector3f::UnitY());
		Cam.lookAt(Vector3f(10.0f, 5.0f, 35.0f), Vector3f(0.0f, 4.0f, 25.0f), Vector3f::UnitY());
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		// initialize sun (key lights) and back ground light (fill light)
		Vector3f SunPos = Vector3f(-15.0f, 80.0f, 40.0f);
		Vector3f SunLookAt = Vector3f(0.0f, 0.0f, -10.0f);
		Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -20.0f);
		Vector3f FillLightPos = Vector3f(25.0f, 30.0f, 30.0f);
		DirectionalLight Sun;
		DirectionalLight  Fill;
		PointLight BGLight;
		Sun.init(SunPos, (SunLookAt - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 4.0f);
		// sun will cast shadows
		int32_t ShadowMapSize = 4096;
		Sun.initShadowCasting(ShadowMapSize, ShadowMapSize, GraphicsUtility::orthographicProjection(40.0f, 40.0f, 0.1f, 1000.0f));
		BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 2.5f, Vector3f(0.0f, 0.0f, 0.0f));
		Fill.init(FillLightPos, (SunLookAt - FillLightPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 3.5f);

		// set camera and lights
		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);
		RDev.addLight(&BGLight);
		RDev.addLight(&Fill);

		// load skydome and a textured cube
		T3DMesh<float> M;
		StaticActor Skydome;
		SkeletalActor Captured;
		SkeletalActor Synth;
		SkeletalActor Style;
		SkeletalAnimationController ControllerCaptured;
		SkeletalAnimationController ControllerSynth;
		SkeletalAnimationController ControllerStyle;

		SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		M.computePerVertexNormals();
		Skydome.init(&M);
		M.clear();

		// initialize skeletal actor (Eric) and its animation controller
		SAssetIO::load("MyAssets/DoubleCaptured.glb", &M);
		SceneUtilities::setMeshShader(&M, 0.6f, 0.04f);
		// male textures
		//M.getMaterial(0)->TexAlbedo = "Assets/tmp/MHTextures/young_lightskinned_male_diffuse2.png";
		//M.getMaterial(1)->TexAlbedo = "Assets/tmp/MHTextures/brown_eye.png";
		//M.getMaterial(2)->TexAlbedo = "Assets/tmp/MHTextures/male_casualsuit04_diffuse.png";
		//M.getMaterial(3)->TexAlbedo = "Assets/tmp/MHTextures/shoes06_diffuse.png";

		// female textures
		M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
		M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.png";
		M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
		M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";
		M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.png";

		M.computePerVertexNormals();
		//M.computePerVertexTangents();
		ControllerCaptured.init(&M);
		Captured.init(&M, &ControllerCaptured);
		M.clear();

		SAssetIO::load("MyAssets/DoubleSynth.glb", &M);
		SceneUtilities::setMeshShader(&M, 0.6f, 0.04f);
		M.computePerVertexNormals();
		M.computePerVertexTangents();
		M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
		M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.png";
		M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
		M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";
		M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.png";
		ControllerSynth.init(&M);
		Synth.init(&M, &ControllerSynth);
		M.clear();

		SAssetIO::load("MyAssets/DoubleStylized.glb", &M);
		SceneUtilities::setMeshShader(&M, 0.7f, 0.04f);
		M.computePerVertexNormals();
		M.computePerVertexTangents();
		M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
		M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.png";
		M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
		M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";
		M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.png";
		ControllerStyle.init(&M);
		Style.init(&M, &ControllerStyle);
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

		// add skeletal actor to scene graph (Eric)
		SGNGeometry CapturedSGN;
		SGNGeometry StyleSGN;
		SGNGeometry SynthSGN;
		SGNTransformation CapturedTransformSGN;
		SGNTransformation StyleTransformSGN;
		SGNTransformation SynthTransformSGN;

		Vector3f ModelPos = Vector3f(0.0f, 0.0f, 0.0f);
		Vector3f Offset = Vector3f(0.0f, 0.0f, -5.0f);

		float Sc = 0.05f;

		Quaternionf Rot;
		Rot = Quaternionf::Identity();

	
		CapturedTransformSGN.init(&RootSGN, ModelPos + 2 * Offset, Rot, Vector3f(Sc, Sc, Sc));
		CapturedSGN.init(&CapturedTransformSGN, &Captured);

		StyleTransformSGN.init(&RootSGN, ModelPos + Offset, Rot, Vector3f(Sc, Sc, Sc));
		StyleSGN.init(&StyleTransformSGN, &Style);

		SynthTransformSGN.init(&RootSGN, ModelPos, Rot, Vector3f(Sc, Sc, Sc));
		SynthSGN.init(&SynthTransformSGN, &Synth);

		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;

		// check wheter a GL error occurred
		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		SkeletalAnimationController::Animation* pAnimFront = nullptr;
		SkeletalAnimationController::Animation* pAnimBack = nullptr;

		bool RepeatAnim = false;
		float LastAnimSpeed = 16.666f;

		// start main loop
		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SG.update(FPS / 60.0f);

			// this will progress all active skeletal animations for this controller
			ControllerCaptured.update(60.0f/FPS);
			ControllerSynth.update(60.0f/FPS);
			ControllerStyle.update(60.0f/FPS);

			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());

			// if user hits key 1, animation will be played
			// if user also presses shift, animation speed is doubled
			float AnimationSpeed = 16.666f;
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed /= 2.0f;
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				Captured.activeAnimation(ControllerCaptured.createAnimation(0, AnimationSpeed, 0.0f));
				Synth.activeAnimation(ControllerSynth.createAnimation(0, AnimationSpeed, 0.0f));
				Style.activeAnimation(ControllerStyle.createAnimation(0, AnimationSpeed, 0.0f));
				LastAnimSpeed = AnimationSpeed;
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {	
				RepeatAnim = !RepeatAnim;		
			}

			if (RepeatAnim && (Style.activeAnimation() == nullptr || Captured.activeAnimation() == nullptr || Synth.activeAnimation() == nullptr)) {
				Captured.activeAnimation(ControllerCaptured.createAnimation(0, LastAnimSpeed, 0.0f));
				Synth.activeAnimation(ControllerSynth.createAnimation(0, LastAnimSpeed, 0.0f));
				Style.activeAnimation(ControllerStyle.createAnimation(0, LastAnimSpeed, 0.0f));
			}

			
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_3, true)) {
				bool En;
				SynthSGN.enabled(nullptr, &En);
				SynthSGN.enable(true, !En);
			}
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) {
				bool En;
				StyleSGN.enabled(nullptr, &En);
				StyleSGN.enable(true, !En);
			}
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_5, true)) {
				bool En;
				CapturedSGN.enabled(nullptr, &En);
				CapturedSGN.enable(true, !En);
			}
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_6, true)) {
				
			}
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_7, true)) {
				
			}
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_8, true)) {
				
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

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_F2, true)) {
				static uint32_t ScreenshotCount = 0;
				T2DImage<uint8_t> ColorBuffer;
				GraphicsUtility::retrieveFrameBuffer(&ColorBuffer);
				ScreenshotCount++;
				AssetIO::store("Screenshot_" + to_string(ScreenshotCount) + ".jpg", &ColorBuffer);
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
		}//while[main loop]

		pSMan->release();

	}//exampleMinimumGraphicsSetup

}

#endif