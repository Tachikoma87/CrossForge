/*****************************************************************************\
*                                                                           *
* File(s): exampleMinimumGraphicsSetup.hpp                                            *
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
#ifndef __CFORGE_IMUINPUTDEVICETESTSCENE_HPP__
#define __CFORGE_IMUINPUTDEVICETESTSCENE_HPP__

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

#include "../Internet/UDPSocket.h"
#include "../Internet/IMUPackage.hpp"


#include "../Misc/IMUCameraController.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	void imuInputDeviceTestScene(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge - IMU Input Device Test Scene";
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

		//UDPSocket::startup();

		/*UDPSocket IMUServer;
		IMUServer.begin(UDPSocket::TYPE_SERVER, 25000);*/

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
		LC.ShadowBias = 0.0001f;
		LC.ShadowMapCount = 1;
		pSMan->configShader(LC);

		// initialize camera
		VirtualCamera Cam;
		Cam.init(Vector3f(0.0f, 0.25f, 8.0f), Vector3f::UnitY());
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		// initialize sun (key lights) and back ground light (fill light)
		Vector3f SunPos = Vector3f(-15.0f, 125.0f, 200.0f);
		Vector3f BGLightPos = Vector3f(0.0f, 50.0f, -100.0f);
		DirectionalLight Sun;
		PointLight BGLight;
		Sun.init(SunPos, (Vector3f(0.0f, 0.0f, 0.0f) - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 15.0f);
		// sun will cast shadows
		Sun.initShadowCasting(4*1024, 4*1024, GraphicsUtility::orthographicProjection(175.0f, 175.0f, 0.1f, 1000.0f));
		BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 2.0f, Vector3f(0.0f, 0.0f, 0.0f));

		// set camera and lights
		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);
		RDev.addLight(&BGLight);

		// load skydome and a textured cube
		T3DMesh<float> M;
		StaticActor Skydome;
		StaticActor Cube;

		SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		M.computePerVertexNormals();
		Skydome.init(&M);
		M.clear();

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

		StaticActor Tree1;
		StaticActor Tree2;

		AssetIO::load("Assets/tmp/lowpolytree.obj", &M);
		SceneUtilities::setMeshShader(&M, 0.7f, 0.94f);
		M.computePerVertexNormals();
		Tree1.init(&M);
		M.clear();

		AssetIO::load("Assets/tmp/Lowpoly_tree_sample.obj", &M);
		SceneUtilities::setMeshShader(&M, 0.7f, 0.94f);
		M.computePerVertexNormals();
		Tree2.init(&M);
		M.clear();

		// add skydome
		SGNGeometry SkydomeSGN;
		SkydomeSGN.init(&RootSGN, &Skydome);
		SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

		// generate a forest
		const uint32_t TreeCount = 500;
		SGNGeometry* pTreeNodes = new SGNGeometry[TreeCount];
		SGNTransformation* pTreeTransNodes = new SGNTransformation[TreeCount];

		float MinPlane = -150.0f;
		float MaxPlane = 150.0f;

		for (uint32_t i = 0; i < TreeCount; ++i) {

			// placement in world
			Vector3f Pos;
			Pos.x() = CoreUtility::randRange(MinPlane, MaxPlane);
			Pos.y() = 0.0f;
			Pos.z() = CoreUtility::randRange(MinPlane, MaxPlane);

			float Scaling = CoreUtility::randRange(1.8f, 2.4f);

			Quaternionf RotationY;
			RotationY = AngleAxisf(GraphicsUtility::degToRad(CoreUtility::randRange(0.0f, 360.0f)), Vector3f::UnitY());

			pTreeTransNodes[i].init(&RootSGN);
			pTreeTransNodes[i].translation(Pos);
			pTreeTransNodes[i].scale(Vector3f(Scaling, Scaling, Scaling));
			pTreeTransNodes[i].rotation(RotationY);


			if (CoreUtility::rand() % 5 != 0) {
				Vector3f StaticOffset = Vector3f(0.0f, 1.8f * Scaling, 0.0f);
				pTreeNodes[i].init(&pTreeTransNodes[i], &Tree1, StaticOffset);
			}
			else {
				Vector3f StaticOffset = Vector3f(0.0f, 0.0f * Scaling, 0.0f);
				Vector3f StaticScale = Vector3f(0.15f, 0.15f, 0.15f);
				pTreeNodes[i].init(&pTreeTransNodes[i], &Tree2, StaticOffset, Quaternionf::Identity(), StaticScale);
			}


		}//for[generate trees]


		// add cube
		SGNGeometry CubeSGN;
		SGNTransformation CubeTransformSGN;
		CubeTransformSGN.init(&RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
		CubeSGN.init(&CubeTransformSGN, &Cube);
		CubeSGN.enable(true, false);

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

		uint64_t LastMessage = CoreUtility::timestamp();

		uint8_t Buffer[256];
		uint32_t MsgLength;

		std::string Sender;
		uint16_t Port;

		UDPSocket::startup();
		IMUCamera IMUCam;
		IMUCam.init(25001, 25000, 250, 250);


		while (!RenderWin.shutdown()) {
			
			IMUCam.update(&Cam, 1.0f);

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

				RenderWin.title(WindowTitle + "[" + std::string(Buf) + "]");
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
		}//while[main loop]

		/*Client.end();
		Server.end();*/

		//IMUServer.end();

		UDPSocket::cleanup();
		
		pSMan->release();

	}//exampleMinimumGraphicsSetup

}

#endif