/*****************************************************************************\
*                                                                           *
* File(s): TCPSocket.h and TCPSocket.cpp                                              *
*                                                                           *
* Content: A socket class that manages TCP connections.   *
*                                                   *
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
#ifndef __CFORGE_TCPSOCKET_H__
#define __CFORGE_TCPSOCKET_H__

#include <thread>
#include <queue>
#include <mutex>
#include "../../CForge/Core/CForgeObject.h"

namespace CForge {
	class TCPSocket {
	public:
		enum SocketType : int8_t {
			TYPE_UNKNOWN = -1,
			TYPE_SERVER,
			TYPE_CLIENT,
		};

		// start and clears Winsocket stuff (WSA). Move to engine object later as this is only a temporary solution
		static void startup(void);
		static void cleanup(void);

		TCPSocket(void);
		~TCPSocket(void);

		void begin(SocketType Type, uint16_t Port);
		void end(void);

		void sendData(uint8_t* pData, uint32_t DataSize, int32_t ConnectionID);
		bool recvData(uint8_t* pBuffer, uint32_t* pDataSize, int32_t ConnectionID);
		bool connectTo(std::string IP, uint16_t Port);

		uint32_t activeConnections(void)const;

	protected:
		struct Package {
			uint8_t* pData;
			uint32_t DataSize;
			int32_t ConnectionID;
			

			Package(void) {
				pData = nullptr;
				DataSize = 0;
			}
			~Package(void) {
				if (nullptr != pData) delete[] pData;
				pData = nullptr;
				DataSize = 0;
			}
		};

		struct Connection {
			void* pHandle;	///< Socket handle
			std::string IP;	///< IP
			uint16_t Port;	///< Port
			std::thread* pRecvThread; ///< active data receive method
			bool Close;		///< Whether connection is about to close
			std::queue<Package*> InQueue;
			std::mutex Mutex;
		};

		void acceptThread(void); // accepts incoming connections
		void recvThread(int32_t ConnectionID); // recvs data (one for each connection)


		void* m_pHandle; // Socket handle
		uint16_t m_Port; // Port

		std::thread* m_pAcceptThread;
		//std::queue<Package*> m_InQueue;
		std::vector<Connection*> m_ActiveConnections;

		uint8_t* m_pInBuffer;
		uint8_t* m_pOutBuffer;
		uint32_t m_BufferSize;
	};//Socket

}//name space

#endif 