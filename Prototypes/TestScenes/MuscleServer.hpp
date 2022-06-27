#ifndef __CFORGE_MUSCLESERVER_HPP__
#define __CFORGE_MUSCLESERVER_HPP__

#include "../../CForge/AssetIO/SAssetIO.h"




#include "../../CForge/Internet/TCPSocket.h"

#include <memory>
#include "../../CForge/Internet/InternetFileStream.h"


namespace CForge {




	void muscleServer(void) {


		uint16_t port = 2456;

		// start Server
		TCPSocket server;
		server.begin(TCPSocket::SocketType::TYPE_SERVER, port);
		bool server_active = true;
		uint32_t DataSize=128;
		
		printf("active\n");
		//wait for clients to send data
		while (server_active) {
			uint8_t recvBuffer[128];
			uint8_t recv_Buffer[1];
			uint8_t ack[1];
			uint8_t ackSize=1;
			
			for (uint32_t i = 0; i < server.activeConnections(); ++i) {
				while (server.recvData(recvBuffer, &DataSize, i)) {

					InternetFileStream::receiveFile("Assets/MuscleAnalysisPL/animation.bvh", server, i, recvBuffer, DataSize);
					//start Pipeline
					//  set Path to Blender in bat file
					File pl;
					//std::system(pl.absolute("Assets/MuscleAnalysisPL/pl.bat").c_str());
					//send files Back to Client
					InternetFileStream::sendFile("Assets/MuscleAnalysisPL/animation_scaled.fbx",128,server,i);
					InternetFileStream::sendFile("Assets/MuscleAnalysisPL/3DGaitModel2354-scaled_MuscleAnalysis_FiberForce.sto", 128, server, i);
					
					
				}
			}
		}


		//end



	}
}

#endif