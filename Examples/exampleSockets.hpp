/*****************************************************************************\
*                                                                           *
* File(s): exampleSockets.hpp                                               *
*                                                                           *
* Content: Example scene that creates UDP and TCP Sockets                   *
*          (1 Server + 2 clients) on demand and sends some simple text      *
*          messages, including acknowledge from server.                     *
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
#ifndef __CFORGE_EXAMPLESOCKETS_HPP__
#define __CFORGE_EXAMPLESOCKETS_HPP__


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
#include "SceneUtilities.hpp"

#include "../CForge/Internet/TCPSocket.h"
#include "../CForge/Internet/UDPSocket.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	void exampleSockets(void) {
		SShaderManager* pSMan = SShaderManager::instance();

		std::string WindowTitle = "CForge - Sockets Example";
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


		// UDP variables
		UDPSocket UDPServer;
		UDPSocket UDPClient1;
		UDPSocket UDPClient2;
		int32_t UDPPort = 23456;
		int32_t UDPMsgCounter = 0;
		bool UDPActive = false;

		// TCP variable
		TCPSocket TCPServer;
		TCPSocket TCPClient1;
		TCPSocket TCPClient2;
		int32_t TCPPort = 12345;
		int32_t TCPMsgCounter = 0;
		bool TCPActive = false;

		// some variables required for incoming data
		std::string Sender;
		uint16_t Port;
		uint8_t Buffer[128];
		uint32_t DataSize;

		while (!RenderWin.shutdown()) {

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				try {
					printf("Starting UDP server...");
					UDPServer.begin(UDPSocket::TYPE_SERVER, UDPPort);
					printf("ok\n");
					printf("Starting UDP Clients...\n\tClient 1...");
					UDPClient1.begin(UDPSocket::TYPE_CLIENT, 0);
					printf("ok\n\tClient 2...");
					UDPClient2.begin(UDPSocket::TYPE_CLIENT, 0);
					printf("ok\n");

					UDPActive = true;
				}
				catch (const CrossForgeException& e) {
					SLogger::logException(e);
				}

			}//if[pressed key 1]

			if (UDPActive) {
				// check server data
				while (UDPServer.recvData(Buffer, &DataSize, &Sender, &Port)) {
					printf("Message from %s:%d: %s\n", Sender.c_str(), Port, (char*)Buffer);
					string Response = "Acknowledged - " + std::string((char*)Buffer);
					UDPServer.sendData((uint8_t*)Response.c_str(), Response.length() + 1, Sender, Port);
				}

				if (UDPClient1.recvData(Buffer, &DataSize, &Sender, &Port)) {
					printf("UDP Client 1 received message from %s:%d: %s\n", Sender.c_str(), Port, (char*)Buffer);
				}
				if (UDPClient2.recvData(Buffer, &DataSize, &Sender, &Port)) {
					printf("UDP Client 2 received message from %s:%d: %s\n", Sender.c_str(), Port, (char*)Buffer);
				}


				if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
					string Msg = "UDP Client 1: Message " + std::to_string(UDPMsgCounter++);
					UDPClient1.sendData((uint8_t*)Msg.c_str(), Msg.length() + 1, "127.0.0.1", UDPPort);
				}
				if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_3, true)) {
					string Msg = "UDP Client 2: Message " + std::to_string(UDPMsgCounter++);
					UDPClient2.sendData((uint8_t*)Msg.c_str(), Msg.length() + 1, "127.0.0.1", UDPPort);
				}
			}//if[UDPActive]

			// activate servers and clients on demand
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) {
				try {
					printf("Starting TCP server...");
					TCPServer.begin(TCPSocket::TYPE_SERVER, TCPPort);
					printf("ok\n");
					printf("Starting TCP clients: Client 1...");
					TCPClient1.begin(TCPSocket::TYPE_CLIENT, TCPPort);
					printf("ok...Client 2...");
					TCPClient2.begin(TCPSocket::TYPE_CLIENT, TCPPort);
					printf("ok\n");
					printf("Connecting clients to server...\n");
					// connect to server
					if (TCPClient1.connectTo("127.0.0.1", TCPPort)) printf("\tTCP Client 1 connected to 127.0.0.1:"); printf("%s\n", std::to_string(TCPPort).c_str());
					if (TCPClient2.connectTo("127.0.0.1", TCPPort)) printf("\tTCP Client 2 connected to 127.0.0.1:"); printf("%s\n", std::to_string(TCPPort).c_str());

					TCPActive = true;
				}
				catch (const CrossForgeException& e) {
					SLogger::logException(e);
				}
			}//if[pressed key 1]

			if (TCPActive) {
				// check data at server
				for (uint32_t i = 0; i < TCPServer.activeConnections(); ++i) {
					while (TCPServer.recvData(Buffer, &DataSize, i)) {
						auto Info = TCPServer.connectionInfo(i);
						printf("TCP Server received message from %s:%d: %s\n", Info.IP.c_str(), Info.Port, (char*)Buffer);
						string Response = "Acknowledged - " + std::string((char*)Buffer);;
						TCPServer.sendData((uint8_t*)Response.c_str(), Response.length() + 1, i);
					}
				}
				// check client 1 receive status
				if (TCPClient1.recvData(Buffer, &DataSize, 0)) {
					printf("TCP client 1 received message: %s\n", (char*)Buffer);
				}
				if (TCPClient2.recvData(Buffer, &DataSize, 0)) {
					printf("TCP client 2 received message: %s\n", (char*)Buffer);
				}

				if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_5, true)) {
					string Msg = "TCP message " + std::to_string(TCPMsgCounter++);
					TCPClient1.sendData((uint8_t*)Msg.c_str(), Msg.length() + 1, 0);
				}
				if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_6, true)) {
					string Msg = "TCP message " + std::to_string(TCPMsgCounter++);
					TCPClient2.sendData((uint8_t*)Msg.c_str(), Msg.length() + 1, 0);
				}
			}//if[TCP active]



			// render something
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

		// cleanup
		if (TCPActive) {
			TCPClient1.end();
			TCPClient2.end();
			TCPServer.end();
		}

		if (UDPActive) {
			UDPClient1.end();
			UDPClient2.end();
			UDPServer.end();
		}


	}//exampleSocketTestScene

}//name space

#endif