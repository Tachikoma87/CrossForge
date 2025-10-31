#include "UDPSocket.h"

#ifdef WIN32
#include <WinSock2.h>

namespace CForge {

	UDPSocket::UDPSocket(void): CForgeObject("UDPSocket") {
		m_pRecvThread = nullptr;
		m_pHandle = nullptr;
		m_Port = 0;

		m_pInBuffer = nullptr;
		m_BufferSize = 0;
	}//Constructor

	UDPSocket::~UDPSocket(void) {
		end();

	}//Destructor

	void UDPSocket::begin(uint16_t Port) {
		end();

		// create a socket
		SOCKET pSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (INVALID_SOCKET == pSock) throw CForgeExcept("Creating socket failed!");

		m_Port = Port;
		
		sockaddr_in Addr;
		Addr.sin_family = AF_INET;
		Addr.sin_port = htons(m_Port);
		Addr.sin_addr.s_addr = ADDR_ANY;
		int32_t rc = bind(pSock, (sockaddr*)&Addr, sizeof(SOCKADDR_IN));
		if (rc == SOCKET_ERROR) throw CForgeExcept("Binding socket failed. WSA error: " + std::to_string(WSAGetLastError()));

		int32_t s = sizeof(SOCKADDR_IN);
		getsockname(pSock, (sockaddr*)&Addr, &s);

		m_Port = Addr.sin_port;

		m_pHandle = (void*)pSock;

		// start thread
		m_pRecvThread = new std::thread(&UDPSocket::recvThread, this);

		// create Buffer
		m_BufferSize = 2048;
		m_pInBuffer = new uint8_t[m_BufferSize];
	}//begin

	void UDPSocket::end(void) {
		if (nullptr != m_pHandle) closesocket((SOCKET)m_pHandle);
		m_pHandle = nullptr;
		if (m_pRecvThread != nullptr) m_pRecvThread->join();
		m_pRecvThread = nullptr;

		m_pHandle = nullptr;

		if (nullptr != m_pInBuffer) delete[] m_pInBuffer;
		m_BufferSize = 0;

		m_pInBuffer = nullptr;

		while (!m_InQueue.empty()) {
			auto* pMsg = m_InQueue.front();
			delete pMsg;
			m_InQueue.pop();
		}

	}//end

	void UDPSocket::sendData(uint8_t* pData, uint32_t DataSize, std::string IP, uint16_t Port) {
		if (nullptr == m_pHandle) throw CForgeExcept("Socket not valid!");

		SOCKET pSock = (SOCKET)m_pHandle;

		sockaddr_in RecvAddr;
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port = htons(Port);
		RecvAddr.sin_addr.s_addr = inet_addr(IP.c_str());

		if (SOCKET_ERROR == sendto(pSock, (char*)pData, DataSize, 0, (sockaddr*)&RecvAddr, sizeof(RecvAddr))) {
			throw CForgeExcept("Sending message to " + IP + ":" + std::to_string(Port) + " failed!");
		}

	}//send

	bool UDPSocket::recvData(uint8_t* pBuffer, uint32_t BufferSize, uint32_t* pDataSize, std::string* pSender, uint16_t* pPort) {
		if (m_InQueue.empty()) return false;

		m_Mutex.lock();
		Package* pRval = m_InQueue.front();
		if(BufferSize >= pRval->DataSize) m_InQueue.pop();
		m_Mutex.unlock();

		if (BufferSize < pRval->DataSize) throw CForgeExcept("Specified buffer is too small!");

		(*pDataSize) = pRval->DataSize;
		if (nullptr != pSender) (*pSender) = pRval->IP;
		if (nullptr != pPort) (*pPort) = pRval->Port;
		memcpy(pBuffer, pRval->pData, pRval->DataSize);
		delete pRval;
		return true;
	}//getMessage

	uint16_t UDPSocket::port(void)const {
		return m_Port;
	}//port


	void UDPSocket::recvThread(void) {
		SOCKET pSock = (SOCKET)m_pHandle;

		while (nullptr != m_pHandle) {
			sockaddr_in AddrIn;
			int32_t AddrLen;

			AddrIn.sin_family = AF_INET;
			AddrIn.sin_port = htons(m_Port);
			AddrIn.sin_addr.s_addr = INADDR_ANY;
			int32_t Len = sizeof(AddrIn);

			int32_t MsgLength = recvfrom(pSock, (char*)m_pInBuffer, m_BufferSize, 0, (sockaddr*)&AddrIn, &Len);
			if (MsgLength > 0) {
				Package* pP = new Package();
				pP->pData = new uint8_t[MsgLength];
				memcpy(pP->pData, m_pInBuffer, MsgLength);
				pP->DataSize = MsgLength;
				pP->IP = inet_ntoa(AddrIn.sin_addr);
				pP->Port = ntohs(AddrIn.sin_port);
				m_Mutex.lock();
				m_InQueue.push(pP);
				m_Mutex.unlock();
			}
		}//while[do not leave thread]

	}//socketThread

}//name space

#elif defined unix

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


namespace CForge {

	UDPSocket::UDPSocket(void): CForgeObject("UDPSocket") {
		m_pRecvThread = nullptr;
		m_pHandle = nullptr;

		m_pInBuffer = nullptr;
		m_pOutBuffer = nullptr;
		m_BufferSize = 0;
	}//Constructor

	UDPSocket::~UDPSocket(void) {
		end();
	}//Destructor

	void UDPSocket::begin(uint16_t Port) {
		end();

		// create a socket
		int64_t Sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (-1 == Sock) throw CForgeExcept("Creating socket failed!");

		m_Port = Port;

		
		// bind to port
		sockaddr_in Addr;
		memset(&Addr, 0, sizeof(Addr));
		Addr.sin_family = AF_INET;
		Addr.sin_port = htons(m_Port);
		Addr.sin_addr.s_addr = INADDR_ANY;
		int32_t rc = bind(Sock, (sockaddr*)&Addr, sizeof(sockaddr));
		if (-1 == rc) throw CForgeExcept("Binding socket failed.");
		
		int32_t s = sizeof(SOCKADDR_IN);
		getsockname(pSock, (sockaddr*)&Addr, &s);
		m_Port = Addr.sin_port;

		m_pHandle = (void*)Sock;

		// start thread
		m_pRecvThread = new std::thread(&UDPSocket::recvThread, this);

		// create Buffer
		m_BufferSize = 2048;
		m_pInBuffer = new uint8_t[m_BufferSize];
		m_pOutBuffer = new uint8_t[m_BufferSize];
	}//begin

	void UDPSocket::end(void) {
		if (nullptr != m_pHandle) {
			int64_t Sock = (int64_t)m_pHandle;
			m_pHandle = nullptr;
			shutdown(Sock, SHUT_RDWR);
			close(Sock);
		}

		if (m_pRecvThread != nullptr) m_pRecvThread->join();
		m_pRecvThread = nullptr;

		m_pHandle = nullptr;

		if (nullptr != m_pInBuffer) delete[] m_pInBuffer;
		if (nullptr != m_pOutBuffer) delete[] m_pOutBuffer;
		m_BufferSize = 0;

		m_pInBuffer = nullptr;
		m_pOutBuffer = nullptr;

		while (!m_InQueue.empty()) {
			auto* pMsg = m_InQueue.front();
			delete pMsg;
			m_InQueue.pop();
		}
	}//end

	void UDPSocket::sendData(uint8_t* pData, uint32_t DataSize, std::string IP, uint16_t Port) {
		if (nullptr == m_pHandle) throw CForgeExcept("Socket not valid!");

		int64_t Sock = (int64_t)m_pHandle;

		sockaddr_in RecvAddr;
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port = htons(Port);
		RecvAddr.sin_addr.s_addr = inet_addr(IP.c_str());

		if (-1 == sendto(Sock, (char*)pData, DataSize, 0, (sockaddr*)&RecvAddr, sizeof(RecvAddr))) {
			throw CForgeExcept("Sending message to " + IP + ":" + std::to_string(Port) + " failed!");
		}
	}//sendData

	bool UDPSocket::recvData(uint8_t* pBuffer, uint32_t* pDataSize, std::string* pSender, uint16_t* pPort) {
		if (m_InQueue.empty()) return false;

		m_Mutex.lock();
		Package* pRval = m_InQueue.front();
		m_InQueue.pop();
		m_Mutex.unlock();

		(*pDataSize) = pRval->DataSize;
		if (nullptr != pSender) (*pSender) = pRval->IP;
		if (nullptr != pPort) (*pPort) = pRval->Port;
		memcpy(pBuffer, pRval->pData, pRval->DataSize);
		delete pRval;
		return true;
	}//recvData

	uint16_t UDPSocket::port(void)const {
		return m_Port;
	}//port

	void UDPSocket::recvThread(void) {
		int64_t pSock = (int64_t)m_pHandle;

		while (nullptr != m_pHandle) {
			sockaddr_in AddrIn;
			int32_t AddrLen;

			AddrIn.sin_family = AF_INET;
			AddrIn.sin_port = htons(m_Port);
			AddrIn.sin_addr.s_addr = INADDR_ANY;
			socklen_t Len = sizeof(AddrIn);

			int32_t MsgLength = recvfrom(pSock, (char*)m_pInBuffer, m_BufferSize, 0, (sockaddr*)&AddrIn, &Len);
			if (MsgLength > 0) {
				Package* pP = new Package();
				pP->pData = new uint8_t[MsgLength];
				memcpy(pP->pData, m_pInBuffer, MsgLength);
				pP->DataSize = MsgLength;
				pP->IP = inet_ntoa(AddrIn.sin_addr);
				pP->Port = ntohs(AddrIn.sin_port);
				m_Mutex.lock();
				m_InQueue.push(pP);
				m_Mutex.unlock();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}//while[do not leave thread]

	}//socketThread

}//name space


#endif