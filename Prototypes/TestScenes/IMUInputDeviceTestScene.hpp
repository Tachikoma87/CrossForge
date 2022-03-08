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

#include "../../CForge/Graphics/Actors/SkyboxActor.h"

#include "../Internet/UDPSocket.h"
#include "../Internet/IMUPackage.hpp"


#include "../Misc/IMUCameraController.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	struct Rect {
		Vector2f Min;
		Vector2f Max;

		inline bool pointInside(Vector2f P) const{
			bool Rval = true;
			if (P.x() > Max.x()) Rval = false;
			if (P.y() > Max.y()) Rval = false;
			if (P.x() < Min.x()) Rval = false;
			if (P.y() < Min.y()) Rval = false;
			return Rval;
		}
	};

	struct Sphere {
		Vector3f Position;
		float Radius2;

		inline bool pointInside(Vector3f P) const {
			const Vector3f Vec = (P - Position);
			return Vec.dot(Vec) < Radius2;
		}
	};

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
		LC.PCFSize = 2;
		LC.ShadowBias = 0.0001f;
		LC.ShadowMapCount = 1;
		pSMan->configShader(LC);


		// initialize camera
		VirtualCamera Cam;
		Cam.init(Vector3f(0.0f, 0.25f, 8.0f), Vector3f::UnitY());
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		// initialize sun (key lights) and back ground light (fill light)
		Vector3f SunPos = Vector3f(-15.0f, 625.0f, 500.0f);
		Vector3f BGLightPos = Vector3f(0.0f, 10.0f, -500.0f);
		DirectionalLight Sun;
		PointLight BGLight;
		Sun.init(SunPos, (Vector3f(0.0f, 0.0f, 0.0f) - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 15.0f);
		// sun will cast shadows
		uint32_t ShadowMapSize = 4 * 1024;
		Sun.initShadowCasting(ShadowMapSize, ShadowMapSize, GraphicsUtility::orthographicProjection(575.0f, 575.0f, 5.0f, 1500.0f));
		BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f, Vector3f(0.0f, 0.0f, 0.0f));

		// set camera and lights
		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);
		RDev.addLight(&BGLight);

		// load skydome and a textured cube
		T3DMesh<float> M;
		StaticActor Skydome;
		StaticActor Cube;

		//SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		SAssetIO::load("MyAssets/TexturedGround.fbx", &M);

		for (uint8_t i = 0; i < 4; ++i) M.textureCoordinate(i) *= 10.0f;

		SceneUtilities::setMeshShader(&M, 0.0f, 0.04f);
		M.getMaterial(0)->Color = 0.75f * Vector4f(0.75f, 0.85f, 0.75f, 1.0f);
		M.getMaterial(0)->TexAlbedo = "MyAssets/ForestGround.jpg";
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

		vector<string> ClearSky;
		/*ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_right.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_left.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_up.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_down.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_back.png");
		ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_front.png");*/

		ClearSky.push_back("MyAssets/Cloudy/bluecloud_rt.jpg");
		ClearSky.push_back("MyAssets/Cloudy/bluecloud_lf.jpg");
		ClearSky.push_back("MyAssets/Cloudy/bluecloud_up.jpg");
		ClearSky.push_back("MyAssets/Cloudy/bluecloud_dn.jpg");
		ClearSky.push_back("MyAssets/Cloudy/bluecloud_ft.jpg");
		ClearSky.push_back("MyAssets/Cloudy/bluecloud_bk.jpg");


		SkyboxActor Skybox;
		Skybox.init(ClearSky[0], ClearSky[1], ClearSky[2], ClearSky[3], ClearSky[4], ClearSky[5]);

		// set initialize color adjustment values
		Skybox.brightness(1.15f);
		Skybox.contrast(1.0f);
		Skybox.saturation(1.2f);

		SceneGraph SkyboxSG;
		SGNTransformation SkyboxTransSGN;
		SGNGeometry SkyboxGeomSGN;

		// create scene graph for the Skybox
		SkyboxTransSGN.init(nullptr);
		SkyboxGeomSGN.init(&SkyboxTransSGN, &Skybox);
		SkyboxSG.init(&SkyboxTransSGN);
		
		Quaternionf Rot;
		Rot = AngleAxisf(GraphicsUtility::degToRad(-0.25f / 60.0f), Vector3f::UnitY());
		SkyboxTransSGN.rotationDelta(Rot);

		StaticActor Tree1;
		StaticActor Tree2;
		StaticActor Coin;

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

		AssetIO::load("MyAssets/StarCoin.glb", &M);
		SceneUtilities::setMeshShader(&M, 0.2f, 0.5f);
		M.computePerVertexNormals();
		for (uint32_t i = 0; i < M.materialCount(); ++i) {
			M.getMaterial(i)->TexAlbedo = "MyAssets/MaterialStar_baseColor.jpeg";
			M.getMaterial(i)->TexNormal = "";
		}
		Coin.init(&M);
		M.clear();

		// add skydome
		SGNGeometry SkydomeSGN;
		SkydomeSGN.init(&RootSGN, &Skydome);
		Rot;
		Rot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
		SkydomeSGN.rotation(Rot);
		SkydomeSGN.scale(Vector3f(600.0f, 600.0f, 1.0f));

		// generate a forest
		const uint32_t TreeCount = 500;
		SGNGeometry* pTreeNodes = new SGNGeometry[TreeCount];
		SGNTransformation* pTreeTransNodes = new SGNTransformation[TreeCount];
		Sphere* pTreeSpheres = new Sphere[TreeCount];

		float MinPlane = -200.0f;
		float MaxPlane = 200.0f;

		for (uint32_t i = 0; i < TreeCount; ++i) {
			// placement in world
			Vector3f Pos;
			Pos.x() = CoreUtility::randRange(MinPlane, MaxPlane);
			Pos.y() = 0.0f;
			Pos.z() = CoreUtility::randRange(MinPlane, MaxPlane);

			float Scaling = CoreUtility::randRange(3.8f, 8.4f);

			Quaternionf RotationY;
			RotationY = AngleAxisf(GraphicsUtility::degToRad(CoreUtility::randRange(0.0f, 360.0f)), Vector3f::UnitY());

			pTreeTransNodes[i].init(&RootSGN);
			pTreeTransNodes[i].translation(Pos);
			pTreeTransNodes[i].scale(Vector3f(Scaling, Scaling, Scaling));
			pTreeTransNodes[i].rotation(RotationY);

			if (CoreUtility::rand() % 5 != 0) {
				Vector3f StaticOffset = Vector3f(0.0f, 1.8f * Scaling, 0.0f);
				pTreeNodes[i].init(&pTreeTransNodes[i], &Tree1, StaticOffset);
				pTreeSpheres[i].Position = Pos + Vector3f(0.0f, 1.0f, 0.0f);;
				const float r = 2.0f + Scaling / 8.0f;;
				pTreeSpheres[i].Radius2 = r * r;
			}
			else {
				Vector3f StaticOffset = Vector3f(0.0f, 0.0f * Scaling, 0.0f);
				Vector3f StaticScale = Vector3f(0.15f, 0.15f, 0.15f);
				pTreeNodes[i].init(&pTreeTransNodes[i], &Tree2, StaticOffset, Quaternionf::Identity(), StaticScale);
				pTreeSpheres[i].Position = Pos + Vector3f(0.0f, 1.0f, 0.0f);
				const float r = 2.5f;
				pTreeSpheres[i].Radius2 = r * r;
			}

			

		}//for[generate trees]

		// generate coins
		uint32_t CoinCount = 200;
		SGNGeometry* pCoinNodes = new SGNGeometry[CoinCount];
		SGNTransformation* pCoinTransNodes = new SGNTransformation[CoinCount];
		Sphere* pCoinBS = new Sphere[CoinCount];

		float CoinScale = 1.25f;
		Quaternionf CoinRotDelta;
		CoinRotDelta = AngleAxisf(GraphicsUtility::degToRad(180.0f / 60.0f), Vector3f::UnitY());

		for (uint32_t i = 0; i < CoinCount; ++i) {
			// placement in world
			Vector3f Pos;
			Pos.x() = CoreUtility::randRange(MinPlane, MaxPlane);
			Pos.y() = 1.5f;
			Pos.z() = CoreUtility::randRange(MinPlane, MaxPlane);

			Quaternionf RotationY;
			RotationY = AngleAxisf(GraphicsUtility::degToRad(CoreUtility::randRange(0.0f, 360.0f)), Vector3f::UnitY());

			pCoinTransNodes[i].init(&RootSGN);
			pCoinTransNodes[i].translation(Pos);
			//pCoinTransNodes[i].scale(Vector3f(Scaling, Scaling, Scaling));
			pCoinTransNodes[i].rotation(RotationY);
			pCoinTransNodes[i].rotationDelta(CoinRotDelta);

			pCoinNodes[i].init(&pCoinTransNodes[i], &Coin, Vector3f::Zero(), Quaternionf::Identity(), Vector3f(CoinScale, CoinScale, CoinScale));

			pCoinBS[i].Position = Pos;
			const float r = 2.0f;
			pCoinBS[i].Radius2 = r * r;

		}


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
		IMUCameraController IMUCam;
		IMUCam.init(25001, 25000, 200);

		bool Flying = false;

		uint32_t PlayerScore = 0;

		while (!RenderWin.shutdown()) {
			
			RenderWin.update();
			SG.update(60.0f/FPS);
			SkyboxSG.update(60.0f / FPS);

			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse());
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) Flying = !Flying;
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) IMUCam.calibrate();

			Vector3f Pos = Cam.position();
			Pos.x() = std::max(MinPlane, Pos.x());
			Pos.x() = std::min(MaxPlane, Pos.x());
			if(!Flying) Pos.y() = 0.85f;
			Pos.z() = std::max(MinPlane, Pos.z());
			Pos.z() = std::min(MaxPlane, Pos.z());

			Cam.position(Pos);

			IMUCam.update(&Cam, 60.0f / FPS);

			Pos = Cam.position();

			// player collision with tree?
			for (uint32_t i = 0; i < TreeCount; ++i) {
				if (pTreeSpheres[i].pointInside(Pos)) {
					// set player outside sphere
					Vector3f V = pTreeSpheres[i].Position + std::sqrtf(pTreeSpheres[i].Radius2) * (Pos - pTreeSpheres[i].Position).normalized();	
					Pos.x() = V.x();
					Pos.z() = V.z();
					Cam.position(Pos);
				}
			}

			// player within coin range?
			for (uint32_t i = 0; i < CoinCount; ++i) {
				bool En;
				pCoinNodes[i].enabled(&En, nullptr);
				if (En && pCoinBS[i].pointInside(Pos)) {
					pCoinNodes[i].enable(false, false);
					PlayerScore++;
					printf("Player Score: %d\n", PlayerScore);
				}
			}


			RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
			SG.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			SG.render(&RDev);

			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			SkyboxSG.render(&RDev);

			RenderWin.swapBuffers();

			FPSCount++;
			if (CoreUtility::timestamp() - LastFPSPrint > 1000U) {
				char Buf[64];
				sprintf(Buf, "FPS: %d\n", FPSCount);
				FPS = float(FPSCount);
				FPSCount = 0;
				LastFPSPrint = CoreUtility::timestamp();

				RenderWin.title(WindowTitle + "[" + std::string(Buf) + "] - Score: " + std::to_string(PlayerScore) + " Coins");
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_F2, true)) {
				T2DImage<uint8_t> Img;
				static uint32_t ShotCount = 0;
				GraphicsUtility::retrieveFrameBuffer(&Img, nullptr);
				AssetIO::store("Screenshot_" + std::to_string(++ShotCount) + ".jpg", &Img);
			}

		}//while[main loop]

		IMUCam.clear();

		UDPSocket::cleanup();
		
		pSMan->release();

	}//exampleMinimumGraphicsSetup

}

#endif