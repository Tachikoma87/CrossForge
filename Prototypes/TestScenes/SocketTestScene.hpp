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
#ifndef __CFORGE_SOCKETTESTSCENE_HPP__
#define __CFORGE_SOCKETTESTSCENE_HPP__

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

#include "../../Prototypes/Internet/TCPSocket.h"
#include "../../Prototypes/Internet/UDPSocket.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	void socketTestScene(void) {
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

		SAssetIO::load("MyAssets/ExpTest.fbx", &M);

		SAssetIO::load("MyAssets/sea_shell_MedRes.obj", &M);
		M.computePerVertexNormals();
		SAssetIO::store("MyAssets/Tmp/ExpTest.x", &M);
		
		
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

		UDPSocket::startup();
		/*TCPSocket Client;
		TCPSocket Client2;
		TCPSocket Server;
		Server.begin(TCPSocket::TYPE_SERVER, 1122);
		Client.begin(TCPSocket::TYPE_CLIENT, 1122);
		Client2.begin(TCPSocket::TYPE_CLIENT, 1122);*/

		UDPSocket UDPServer;
		UDPSocket UDPClient;

		UDPServer.begin(UDPSocket::TYPE_SERVER, 2233);
		UDPClient.begin(UDPSocket::TYPE_CLIENT, 0);

		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		uint8_t Buffer[256];
		uint32_t DataSize;
		std::string Sender;
		uint16_t Port;

		uint64_t Start = CoreUtility::timestamp();
		uint64_t Start2 = CoreUtility::timestamp();

		/*try {
			if(Client.connectTo("127.0.0.1", 1122)) printf("Client Connected to localhost:1122\n");
			if (Client2.connectTo("127.0.0.1", 1122)) printf("Client2 connected to localhost:1122\n");
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
		}*/

		while (!RenderWin.shutdown()) {

			if (CoreUtility::timestamp() - Start > 1000) {
				UDPClient.sendData((uint8_t*)"Ping", 5, "127.0.0.1", 2233);
				Start = CoreUtility::timestamp();
			}

			if (UDPServer.recvData(Buffer, &DataSize, &Sender, &Port)) {
				printf("Message from %s:%d: %s\n", Sender.c_str(), Port, (char*)Buffer);
			}


			/*for (uint32_t i = 0; i < Server.activeConnections(); ++i) {
				if (Server.recvData(Buffer, &DataSize, i)) {
					printf("%s\n", (char*)Buffer);
					Server.sendData((uint8_t*)"Pong", 5, i);
				}
			}

			if (CoreUtility::timestamp() - Start > 2000) {
				Client.sendData((uint8_t*)"Ping", 5, 0);
				Start = CoreUtility::timestamp();
			}

			if (CoreUtility::timestamp() - Start2 > 3000) {
				Client2.sendData((uint8_t*)"Ring", 5, 0);
				Start2 = CoreUtility::timestamp();
			}

			if (Client.recvData(Buffer, &DataSize, 0)) {
				printf("%s\n", (char*)Buffer);
			}
			if (Client2.recvData(Buffer, &DataSize, 0)) {
				printf("%s\n", (char*)Buffer);
			}*/

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

		pSMan->release();
		/*Client.end();
		Server.end();*/
		UDPSocket::cleanup();

	}//exampleMinimumGraphicsSetup

}

#endif