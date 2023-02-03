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

#include "../CForge/Internet/TCPSocket.h"
#include "../CForge/Internet/UDPSocket.h"

#include "ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleSocket : public ExampleSceneBase {
	public:
		ExampleSocket(void) {
			m_WindowTitle = "CrossForge Example - Sockets";
			m_WinWidth = 1280;
			m_WinHeight = 720;
			m_FPS = 60.0f;
			m_FPSCount = 0;
		}//Constructor

		~ExampleSocket(void) {

		}//Destructor

		void init(void) override{
			initWindowAndRenderDevice();
			initCameraAndLights();

			T3DMesh<float> M;
			
			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Duck/Duck.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Duck.init(&M);
			M.clear();

			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome	
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add cube
			m_DuckTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
			m_DuckSGN.init(&m_DuckTransformSGN, &m_Duck);
			m_DuckSGN.scale(Vector3f(0.02f, 0.02f, 0.02f));

			// rotate about the y-axis at 45 degree every second
			Quaternionf R;
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			m_DuckTransformSGN.rotationDelta(R);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());
	
		}//initialize

		void clear(void) override{
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

			ExampleSceneBase::clear();
		}//clear

		void mainLoop(void) override{

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
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


				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
					string Msg = "UDP Client 1: Message " + std::to_string(UDPMsgCounter++);
					UDPClient1.sendData((uint8_t*)Msg.c_str(), Msg.length() + 1, "127.0.0.1", UDPPort);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_3, true)) {
					string Msg = "UDP Client 2: Message " + std::to_string(UDPMsgCounter++);
					UDPClient2.sendData((uint8_t*)Msg.c_str(), Msg.length() + 1, "127.0.0.1", UDPPort);
				}
			}//if[UDPActive]

			// activate servers and clients on demand
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) {
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

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_5, true)) {
					string Msg = "TCP message " + std::to_string(TCPMsgCounter++);
					TCPClient1.sendData((uint8_t*)Msg.c_str(), Msg.length() + 1, 0);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_6, true)) {
					string Msg = "TCP message " + std::to_string(TCPMsgCounter++);
					TCPClient2.sendData((uint8_t*)Msg.c_str(), Msg.length() + 1, 0);
				}
			}//if[TCP active]

			// render something
			m_RenderWin.update();
			m_SG.update(m_FPS / 60.0f);

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderWin.swapBuffers();

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());

		}//mainLoop

	protected:
		StaticActor m_Skydome;
		StaticActor m_Duck;
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

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

	};//ExampleSocket

	

}//name space

#endif