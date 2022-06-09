/*****************************************************************************\
*                                                                           *
* File(s): HairTestScene.hpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
*          window, lighting setup, and a single moving object.              *
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
#ifndef __CFORGE_HAIRTESTSCENE_HPP__
#define __CFORGE_HAIRTESTSCENE_HPP__

//#include "../../CForge/AssetIO/SAssetIO.h"
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

#include "../../Examples/SceneUtilities.hpp"

#include "../Hair/SJ_SAssetIO.h"
#include "../Hair/HairActor.h"
#include "../Hair/T3DHair.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	void hairTest(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge Prototype - Hair";
		float FPS = 60.0f;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

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

		// load skydome and hair
		T3DMesh<float> M;
		T3DHair<float> H;
		StaticActor Skydome;
		HairActor HairActor;

		SJ_SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		M.computePerVertexNormals();
		Skydome.init(&M);
		M.clear();


		SJ_SAssetIO::load("MyAssets/wStraight.hair", &H);
		HairActor.init(&H);
		H.clear();

		// build scene graph
		SceneGraph SG;
		SGNTransformation RootSGN;
		RootSGN.init(nullptr);
		SG.init(&RootSGN);

		// add skydome
		SGNGeometry SkydomeSGN;
		SkydomeSGN.init(&RootSGN, &Skydome);
		SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

		// add hair
		SGNGeometry HairSGN;
		SGNTransformation HairTransformSGN;
		HairTransformSGN.init(&RootSGN, Vector3f(0.0f, 3.0f, 0.0f), Quaternionf(0.0, 0.0, 0.7071, 0.7071), Vector3f(0.05f, 0.05f, 0.05f));
		HairSGN.init(&HairTransformSGN, &HairActor);

		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;

		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SG.update(FPS / 60.0f);

			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());

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

				RenderWin.title(WindowTitle + " [" + std::string(Buf) + "]");
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
		}//while[main loop]

		pSMan->release();

	}

}

#endif