/*****************************************************************************\
*                                                                           *
* File(s): SkyboxTestScene.hpp                                            *
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
#ifndef __CFORGE_SKYBOXTESTSCENE_HPP__
#define __CFORGE_SKYBOXTESTSCENE_HPP__

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

#include "../../Examples/SceneUtilities.hpp" 

#include "../../CForge/Graphics/Actors/SkyboxActor.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	void skyboxTestScene(void) {
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
		gladLoadGL();

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
		StaticActor Cube;

		SAssetIO::load("Assets/SimpleSkydome.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		M.computePerVertexNormals();
		Skydome.init(&M);
		M.clear();

		SAssetIO::load("Assets/TexturedCube.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.1f, 0.04f);
		M.computePerVertexNormals();
		Cube.init(&M);
		M.clear();

		// build scene graph
		SceneGraph SG;
		SGNTransformation RootSGN;
		RootSGN.init(nullptr);
		SG.init(&RootSGN);

		// add skydome
		SGNGeometry SkydomeSGN;
		//SkydomeSGN.init(&RootSGN, &Skydome);
		//SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

		// add cube
		SGNGeometry CubeSGN;
		SGNTransformation CubeTransformSGN;
		CubeTransformSGN.init(&RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
		CubeSGN.init(&CubeTransformSGN, &Cube);


		// rotate about the y-axis at 45 degree every second
		Quaternionf R;
		R = AngleAxisf(GraphicsUtility::degToRad(45.0f / 60.0f), Vector3f::UnitY());
		CubeTransformSGN.rotationDelta(R);

		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;

		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());


		SkyboxActor Skybox;
		std::vector<string> BoxTexs;
		/*BoxTexs.push_back("MyAssets/skybox/right.jpg");
		BoxTexs.push_back("MyAssets/skybox/left.jpg");
		BoxTexs.push_back("MyAssets/skybox/top.jpg");
		BoxTexs.push_back("MyAssets/skybox/bottom.jpg");
		BoxTexs.push_back("MyAssets/skybox/back.jpg");
		BoxTexs.push_back("MyAssets/skybox/front.jpg");		*/

		/*BoxTexs.push_back("MyAssets/TechnoSkybox/vz_techno_right.png");
		BoxTexs.push_back("MyAssets/TechnoSkybox/vz_techno_left.png");
		BoxTexs.push_back("MyAssets/TechnoSkybox/vz_techno_up.png");
		BoxTexs.push_back("MyAssets/TechnoSkybox/vz_techno_down.png");
		BoxTexs.push_back("MyAssets/TechnoSkybox/vz_techno_back.png");
		BoxTexs.push_back("MyAssets/TechnoSkybox/vz_techno_front.png");*/


		/*BoxTexs.push_back("MyAssets/EmptySpace/vz_empty_space_right.png");
		BoxTexs.push_back("MyAssets/EmptySpace/vz_empty_space_left.png");
		BoxTexs.push_back("MyAssets/EmptySpace/vz_empty_space_up.png");
		BoxTexs.push_back("MyAssets/EmptySpace/vz_empty_space_down.png");
		BoxTexs.push_back("MyAssets/EmptySpace/vz_empty_space_back.png");
		BoxTexs.push_back("MyAssets/EmptySpace/vz_empty_space_front.png");*/

		BoxTexs.push_back("MyAssets/clear/vz_clear_right.png");
		BoxTexs.push_back("MyAssets/clear/vz_clear_left.png");
		BoxTexs.push_back("MyAssets/clear/vz_clear_up.png");
		BoxTexs.push_back("MyAssets/clear/vz_clear_down.png");
		BoxTexs.push_back("MyAssets/clear/vz_clear_back.png");
		BoxTexs.push_back("MyAssets/clear/vz_clear_front.png");

		T2DImage<uint8_t> Img;
		AssetIO::load("MyAssets/clear/vz_clear_right.png", &Img);
		Img.rotate90();
		AssetIO::store("MyAssets/Rotated.jpg", &Img);

		Skybox.init(BoxTexs[0], BoxTexs[1], BoxTexs[2], BoxTexs[3], BoxTexs[4], BoxTexs[5]);
		Skybox.brightness(1.15f);
		Skybox.contrast(1.1f);
		Skybox.saturation(1.2f);

		Quaternionf SkyboxRotDelta = Quaternionf::Identity();
		Quaternionf SkyboxRot = Quaternionf::Identity();
		SkyboxRotDelta = AngleAxisf(GraphicsUtility::degToRad(-1.0f / 60.0f), Vector3f::UnitY());

		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SG.update(60.0f / FPS);

			SkyboxRot *= SkyboxRotDelta;

			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());


			Keyboard* pKeyboard = RenderWin.keyboard();
			float Step = (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) ? -0.05f : 0.05f;
			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) Skybox.brightness(Skybox.brightness() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) Skybox.saturation(Skybox.saturation() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) Skybox.contrast(Skybox.contrast() + Step);


			RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
			SG.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			SG.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			RDev.requestRendering(&Skybox, SkyboxRot, Vector3f::Zero(), Vector3f::Ones());

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