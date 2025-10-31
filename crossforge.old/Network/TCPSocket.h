/*****************************************************************************\
*                                                                           *
* File(s): TCPSocket.h and TCPSocket.cpp                                    *
*                                                                           *
* Content: A socket class that manages TCP connections.                     *
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
#ifndef __CFORGE_TCPSOCKET_H__
#define __CFORGE_TCPSOCKET_H__

#include <thread>
#include <queue>
#include <mutex>
#include "../Core/CForgeObject.h"
#include "../Core/SLogger.h"


namespace CForge {

	/**
	* \brief A socket class that manages TCP connections. Can act as a server or client.
	* \ingroup Network
	* 
	* You can create a TCP server or a client with this class. The actual send and receive operations will be dealt with asynchronously, so these operations are non blocking.
	* \todo Change internal handling of data to  smart pointer.
	* \todo Add container methods to send and receive data.
	* \todo Check if connections are cleared correctly.
	* \todo Add input buffer for each connection.
	* \todo Check for the connection to be closed in the recv thread.
	* \todo Change return value of send/recv to int32_t. Return -1 if connection is dead.
	* \todo Change internal management of connections to std::unordered_map so IDs can stay valid.
	*/
	class CFORGE_API TCPSocket: public CForgeObject {
	public:
		/**
		* \brief Available socket types.
		*/
		enum SocketType : int8_t {
			TYPE_UNKNOWN = -1,	///< Default value.
			TYPE_SERVER,		///< Server type.
			TYPE_CLIENT,		///< Client type.
		};

		/**
		* \brief Structure that holds information about the connection.
		*/
		struct ConnectionInfo {
			std::string IP;	///< IP as string.
			uint16_t Port;	///< The port.

			/**
			* \brief Constructor
			*/
			ConnectionInfo() {
				IP = "0.0.0.0";
				Port = 0;
			}

			/**
			* \brief Destructor
			*/
			~ConnectionInfo() {
				IP = "0.0.0.0";
				Port = 0;
			}

			/**
			* \brief Check whether the connection is valid.
			* 
			* \return True if the connection information is valid, false otherwise.
			*/
			bool valid()const {
				return (IP.compare("0.0.0.0") != 0);
			}
		};

		/**
		* \brief Constructor
		*/
		TCPSocket(void);

		/**
		* \brief Destructor
		*/
		~TCPSocket(void);

		/**
		* \brief Start the TCP connection. Previous connections will be killed.
		* 
		* \param[in] Type of the socket.
		* \param[in] Port Port to bind to.
		*/
		void begin(SocketType Type, uint16_t Port);

		/**
		* \brief End connection and clean up.
		*/
		void end(void);

		/**
		* \brief Send data on an established connection.
		* 
		* \param[in] pData Data to send.
		* \param[in] DataSize Number of bytes to send.
		* \param[in] ConnectionID ID of the connection.
		*/
		void sendData(uint8_t* pData, uint32_t DataSize, int32_t ConnectionID);

		/**
		* \brief Receive next data set from the specified connection.
		* 
		* Incoming data is stored inside a message queue. Use this method to get the next data. Returns false if no more data is available.
		* \param[out] pBuffer Data storage for the message data.
		* \param[in] BufferSize Size in bytes of pBuffer.
		* \param[out] DataSize Number of bytes written to pBuffer.
		* \param[in] ConnectionID ID of the connection to query data from.
		* \throws CrossForgeException Throws an exception if buffer is too small for the data. Data remain in the queue in this case.
		*/
		bool recvData(uint8_t* pBuffer, uint32_t BufferSize, uint32_t* pDataSize, int32_t ConnectionID);

		/**
		* \brief Connect to a specific IP and port.
		* 
		* \param[in] IP IP address as string in the format "x.y.z.w" with \$f x,y,z,w \in [0,255] \$f
		* \param[in] Port The port index to connect to.
		* \return Internal connection ID.
		* \throws CrossForgeException Exception is thrown if connection could not be established.
		*/
		int32_t connectTo(std::string IP, uint16_t Port);

		/**
		* \brief Getter for the connection information of the specified connection ID.
		* 
		* \param[in] ConnectionID ID of the connection.
		* \return Connection information structure of the requested connection.
		*/
		ConnectionInfo connectionInfo(int32_t ConnectionID)const;

		/**
		* \brief Getter for the number of active connections.
		* 
		* \return Number of active connections.
		*/
		uint32_t activeConnections(void)const;

	protected:

		/**
		* \brief Structure that contains all information of a data package.
		*/
		struct Package {
			uint8_t* pData;		///< Data buffer.
			uint32_t DataSize;	///< Size of the data buffer.
			int32_t ConnectionID;	///< Connection ID this package belongs to.

			/**
			* \brief Constructor
			*/
			Package(void) {
				pData = nullptr;
				DataSize = 0;
				ConnectionID = -1;
			}

			/**
			* \brief Destructor
			*/
			~Package(void) {
				if (nullptr != pData) delete[] pData;
				pData = nullptr;
				DataSize = 0;
				ConnectionID = -1;
			}
		};

		/**
		* \brief Structure that stores information of a connection.
		*/
		struct Connection {
			void* pHandle;				///< Socket handle.
			std::string IP;				///< IP address in the form of "x.y.z.w"
			uint16_t Port;				///< Port.
			std::thread* pRecvThread;	///< Active data receive method.
			std::queue<Package*> InQueue;	///< Incoming message queue.
			std::mutex Mutex;			///< Mutex for this connection to make it thread save.

			/**
			* \brief Constructor
			*/
			Connection() {
				pHandle = nullptr;
				IP = "0.0.0.0";
				Port = 0;
				pRecvThread = nullptr;
			}

			/**
			* \brief Destructor
			*/
			~Connection() {
				if (nullptr != pHandle) Logger::log("Connection to " + IP + ":" + std::to_string(Port) + " was not closed properly!", "TCPSocket", Logger::LOGTYPE_ERROR);
				if (nullptr != pRecvThread) Logger::log("Receiving thread of connection " + IP + ":" + std::to_string(Port) + " was not resolved properly!", "TCPSocket", Logger::LOGTYPE_ERROR);
				pHandle = nullptr;
				IP = "0.0.0.0";
				Port = 0;
				pRecvThread = nullptr;
			}

		};

		/**
		* \brief Separate thread method that establishes incoming connections (server only).
		*/
		void acceptThread(void);

		/**
		* \brief Separate thread method that receives data of a connection.
		* 
		* \param[in] ConnectionID Internal ID of an existing connection.
		*/
		void recvThread(int32_t ConnectionID); // recvs data (one for each connection)


		void* m_pHandle; ///< Socket handle.
		uint16_t m_Port; ///< Port.

		std::thread* m_pAcceptThread;					///< Thread handle for the accept method.
		std::vector<Connection*> m_ActiveConnections;	///< Buffer that stores active connections.

		uint8_t* m_pInBuffer;		///< Buffer for incoming data.
		uint32_t m_BufferSize;		///< Size of the input buffer.
	};//Socket

}//name space

#endif 
