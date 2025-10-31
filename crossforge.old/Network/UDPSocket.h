  /*****************************************************************************\
*                                                                           *
* File(s): UDPSocket.h and UDPSocket.cpp                                    *
*                                                                           *
* Content: A socket class that manages a UDP connection.                    *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_UDPSOCKET_H__
#define __CFORGE_UDPSOCKET_H__

#include <thread>
#include <queue>
#include <mutex>
#include "../Core/CForgeObject.h"

namespace CForge {

	/**
	* \brief A socket class that manages a UDP connection.
	* \ingroup Network
	* 
	* \todo Change internal handling to smart pointers.
	* \todo Change data handling to container.
	* \todo Add methods to send/receive container and strings.
	*/
	class CFORGE_API UDPSocket: public CForgeObject {
	public:
		/**
		* \brief Constructor
		*/
		UDPSocket(void);

		/**
		* \brief Destructor
		*/
		~UDPSocket(void);

		/**
		* \brief Initializes a UDP connection.
		* 
		* \param[in] Port The port on which to bind. If 0 the connection will be assigned a port by the system.
		* \throws CrossForgeException Throws exception if connection can not be established.
		*/
		void begin(uint16_t Port);

		/**
		* \brief Shutdown of the connection
		*/
		void end(void);

		/**
		* \brief Send a data package.
		* 
		* \param[in] pData Data buffer to send.
		* \param[in] DataSize Size of the data buffer.
		* \param[in] IP IP address as string in the format "x.y.z.w" with \$f x,y,z,w \in [0,255] \$f
		* \param[in] Port The port on which the data should be send.
		*/
		void sendData(uint8_t* pData, uint32_t DataSize, std::string IP, uint16_t Port);

		/**
		* \brief Retrieve an available data package.
		* 
		* \param[out] pBuffer Buffer the data will be stored in.
		* \param[in] BufferSize Size in bytes of pBuffer.
		* \param[out] pDataSize Number of bytes written.
		* \param[out] pSender IP address where the message originated.
		* \param[out] pPort Port The port on which the message was sent.
		* 
		* \throws CrossForgeException Throws exception if buffer is too small for the data. Data is not lost in this case.
		*/
		bool recvData(uint8_t* pBuffer, uint32_t BufferSize, uint32_t* pDataSize, std::string* pSender, uint16_t* pPort);

		/**
		* \brief Getter for the port.
		* 
		* \return Port the socket is bind to.
		*/
		uint16_t port(void)const;

	protected:

		/**
		* \brief Structure of a data package.
		*/
		struct Package {
			uint8_t* pData;		///< Buffer.
			uint32_t DataSize;	///< Size of the buffer.

			std::string IP;	///< IP address the message came from.
			uint16_t Port;	///< The port the message came from.

			/**
			* \brief Constructor.
			*/
			Package(void) {
				pData = nullptr;
				DataSize = 0;
				IP = "";
				Port = 0;
			}

			/**
			* \brief Destructor
			*/
			~Package(void) {
				if (nullptr != pData) delete[] pData;
				pData = nullptr;
				DataSize = 0;
				IP = "";
				Port = 0;
			}
		};

		/**
		* \brief Separate thread method that handles incoming messages.
		*/
		void recvThread(void);

		void* m_pHandle; ///< Socket handle.
		uint16_t m_Port; ///< The port number.

		uint8_t* m_pInBuffer;	///< Buffer for incoming data.
		uint32_t m_BufferSize;	///< Size of the incoming data buffer.

		std::thread* m_pRecvThread;	///< Thread handle that receives data asynchronously.
		std::queue<Package*> m_InQueue; ///< stores received messages until they are processed.
		std::mutex m_Mutex;				///< Mutex to handle writing/reading to/from the queue.

	};//Socket

}//name space

#endif 