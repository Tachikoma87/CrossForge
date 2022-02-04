/*****************************************************************************\
*                                                                           *
* File(s): UDPSocket.h and UDPSocket.cpp                                              *
*                                                                           *
* Content: A socket class that manages UDP connections.   *
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
#ifndef __CFORGE_UDPSOCKET_H__
#define __CFORGE_UDPSOCKET_H__

#include <thread>
#include <queue>
#include <mutex>
#include "../../CForge/Core/CForgeObject.h"


namespace CForge {
	class UDPSocket {
	public:
		enum SocketType : int8_t {
			TYPE_UNKNOWN = -1,
			TYPE_SERVER = 0,
			TYPE_CLIENT,
		};

		// start and clears Winsocket stuff (WSA). Move to engine object later as this is only a temporary solution
		static void startup(void);
		static void cleanup(void);

		UDPSocket(void);
		~UDPSocket(void);

		void begin(SocketType Type, uint16_t Port);
		void end(void);

		void sendData(uint8_t* pData, uint32_t DataSize, std::string IP, uint16_t Port);
		bool recvData(uint8_t* pBuffer, uint32_t* pDataSize, std::string* pSender, uint16_t* pPort);

	protected:
		struct Package {
			uint8_t* pData;
			uint32_t DataSize;

			std::string IP;
			uint16_t Port;

			Package(void) {
				pData = nullptr;
				DataSize = 0;
				IP = "";
				Port = 0;
			}
			~Package(void) {
				if (nullptr != pData) delete[] pData;
				pData = nullptr;
				DataSize = 0;
				IP = "";
				Port = 0;
			}
		};

		void recvThread(void);


		void* m_pHandle; // Socket handle
		uint16_t m_Port; // Port

		uint8_t* m_pInBuffer;
		uint8_t* m_pOutBuffer;
		uint32_t m_BufferSize;

		std::thread* m_pRecvThread;	///< thread the receives data asynchronously
		std::queue<Package*> m_InQueue; ///< stores received data
		std::mutex m_Mutex;

	};//Socket

}//name space

#endif 