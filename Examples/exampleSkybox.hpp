/*****************************************************************************\
*                                                                           *
* File(s): exampleSkybox.hpp                                            *
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
#ifndef __CFORGE_EXAMPLESKYBOX_HPP__
#define __CFORGE_EXAMPLESKYBOX_HPP__

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
#include "../CForge/Graphics/Actors/SkyboxActor.h"

#include "SceneUtilities.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	void exampleSkybox(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge - Skybox Example";
		float FPS = 60.0f;

		bool const LowRes = false;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		if (LowRes) {
			WinWidth = 720;
			WinHeight = 576;
		}

		// create an OpenGL capable window
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
		StaticActor Cube;

		SAssetIO::load("Assets/ExampleScenes/TexturedCube.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.1f, 0.04f);
		M.computePerVertexNormals();
		Cube.init(&M);
		M.clear();

		// build scene graph
		SceneGraph SG;
		SGNTransformation RootSGN;
		RootSGN.init(nullptr);
		SG.init(&RootSGN);

		// add cube
		SGNGeometry CubeSGN;
		SGNTransformation CubeTransformSGN;
		CubeTransformSGN.init(&RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
		CubeSGN.init(&CubeTransformSGN, &Cube);

		// rotate about the y-axis at 45 degree every second and about the X axis to make it a bit more interesting
		Quaternionf RY, RX;
		RY = AngleAxisf(GraphicsUtility::degToRad(45.0f / 60.0f), Vector3f::UnitY());
		RX = AngleAxisf(GraphicsUtility::degToRad(-25.0f / 60.0f), Vector3f::UnitZ());
		CubeTransformSGN.rotationDelta(RX * RY);

		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;



		/// gather textures for the skyboxes
		vector<string> ClearSky;
		vector<string> EmptySpace;
		vector<string> Techno;
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_right.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_left.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_up.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_down.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_back.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_front.png");

		EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_right.png");
		EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_left.png");
		EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_up.png");
		EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_down.png");
		EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_back.png");
		EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_front.png");

		Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_right.png");
		Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_left.png");
		Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_up.png");
		Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_down.png");
		Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_back.png");
		Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_front.png");

		// create actor and initialize
		SkyboxActor Skybox;
		Skybox.init(ClearSky[0], ClearSky[1], ClearSky[2], ClearSky[3], ClearSky[4], ClearSky[5]);

		// set initialize color adjustment values
		Skybox.brightness(1.15f);
		Skybox.contrast(1.1f);
		Skybox.saturation(1.2f);


		SceneGraph SkyboxSG;
		SGNTransformation SkyboxTransSGN;
		SGNGeometry SkyboxGeomSGN;

		// create scene graph for the Skybox
		SkyboxTransSGN.init(nullptr);
		SkyboxGeomSGN.init(&SkyboxTransSGN, &Skybox);
		SkyboxSG.init(&SkyboxTransSGN);


		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SG.update(60.0f/FPS);
			SkyboxSG.update(60.0f / FPS);

			// handle input for the skybox
			Keyboard* pKeyboard = RenderWin.keyboard();
			float Step = (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) ? -0.05f : 0.05f;
			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) Skybox.brightness(Skybox.brightness() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) Skybox.saturation(Skybox.saturation() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) Skybox.contrast(Skybox.contrast() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT) && pKeyboard->keyPressed(Keyboard::KEY_R)) {
				SkyboxTransSGN.rotationDelta(Quaternionf::Identity());
			}
			else if (pKeyboard->keyPressed(Keyboard::KEY_R, true)) {
				Quaternionf RDelta;
				RDelta = AngleAxisf(GraphicsUtility::degToRad(-2.5f / 60.0f), Vector3f::UnitY());
				SkyboxTransSGN.rotationDelta(RDelta);
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_F1, true)) Skybox.init(ClearSky[0], ClearSky[1], ClearSky[2], ClearSky[3], ClearSky[4], ClearSky[5]);
			if (pKeyboard->keyPressed(Keyboard::KEY_F2, true)) Skybox.init(EmptySpace[0], EmptySpace[1], EmptySpace[2], EmptySpace[3], EmptySpace[4], EmptySpace[5]);
			if (pKeyboard->keyPressed(Keyboard::KEY_F3, true)) Skybox.init(Techno[0], Techno[1], Techno[2], Techno[3], Techno[4], Techno[5]);

			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());

			RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
			SG.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			SG.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			// Skybox should be last thing to render
			SkyboxSG.render(&RDev);

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