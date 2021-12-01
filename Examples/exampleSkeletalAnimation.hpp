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
#ifndef __CFORGE_EXAMPLESKELETALANIMATION_HPP__
#define __CFORGE_EXAMPLESKELETALANIMATION_HPP__

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
#include "../CForge/Graphics/Actors/SkeletalActor.h"

#include "SceneUtilities.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	void exampleSkeletalAnimation(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge - Skeletal Animation Example";
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
		LC.PCFSize = 0;
		LC.ShadowBias = 0.00001f;
		LC.ShadowMapCount = 1;
		pSMan->configShader(LC);

		// initialize camera
		VirtualCamera Cam;
		Cam.init(Vector3f(20.0f, 5.0f, 45.0f), Vector3f::UnitY());
		Cam.lookAt(Vector3f(10.0f, 5.0f, 35.0f), Vector3f(0.0f, 4.0f, 25.0f), Vector3f::UnitY());
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		// initialize sun (key lights) and back ground light (fill light)
		Vector3f SunPos = Vector3f(-25.0f, 50.0f, -20.0f);
		Vector3f SunLookAt = Vector3f(0.0f, 0.0f, 30.0f);
		Vector3f BGLightPos = Vector3f(0.0f, 5.0f, 60.0f);
		DirectionalLight Sun;
		PointLight BGLight;
		Sun.init(SunPos, (SunLookAt - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
		// sun will cast shadows
		Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(20.0f, 20.0f, 0.1f, 1000.0f));
		BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.5f, Vector3f(0.0f, 0.0f, 0.0f));

		// set camera and lights
		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);
		RDev.addLight(&BGLight);

		// load skydome and a textured cube
		T3DMesh<float> M;	
		StaticActor Skydome;
		SkeletalActor Eric;
		SkeletalAnimationController Controller;

		SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		M.computePerVertexNormals();
		Skydome.init(&M);	
		M.clear();
		
		// initialize skeletal actor (Eric) and its animation controller
		SAssetIO::load("Assets/ExampleScenes/Eric_Anim.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.7f, 0.04f);
		M.computePerVertexNormals();
		Controller.init(&M);
		Eric.init(&M, &Controller);
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
		SGNGeometry EricSGN;
		SGNTransformation EricTransformSGN;
		EricTransformSGN.init(&RootSGN, Vector3f(0.0f, 0.0f, 0.0f));
		EricSGN.init(&EricTransformSGN, &Eric);
		EricSGN.scale(Vector3f(0.05f, 0.05f, 0.05f));

		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;

		// check wheter a GL error occurred
		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		// start main loop
		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SG.update(FPS/60.0f);

			// this will progress all active skeletal animations for this controller
			Controller.update(FPS / 60.0f);

			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());

			// if user hits key 1, animation will be played
			// if user also presses shift, animation speed is doubled
			float AnimationSpeed = 1.0f;
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed = 2.0f;
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				SkeletalAnimationController::Animation *pAnim = Controller.createAnimation(0, AnimationSpeed, 0.0f);
				Eric.activeAnimation(pAnim);
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