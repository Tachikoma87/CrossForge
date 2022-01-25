#include "TCPSocket.h"
#include "../../CForge/Core/CoreUtility.hpp"
#include <WinSock2.h>

namespace CForge {

	void TCPSocket::startup(void) {
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			throw CForgeExcept("Failed to initialized winsock. Error Code: " + std::to_string(WSAGetLastError()));
		}
	}//startup

	void TCPSocket::cleanup(void) {
		WSACleanup();
	}//cleanup

	TCPSocket::TCPSocket(void) {
		m_pAcceptThread = nullptr;
		m_pHandle = nullptr;

		m_pInBuffer = nullptr;
		m_pOutBuffer = nullptr;
		m_BufferSize = 0;
	}//Constructor

	TCPSocket::~TCPSocket(void) {
		end();

	}//Destructor

	void TCPSocket::begin(SocketType Type, uint16_t Port) {
		end();

		// create a socket
		SOCKET pSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == pSock) throw CForgeExcept("Creating socket failed!");
		m_Port = Port;

		if (Type == TYPE_SERVER) {
			// bind server to port
			sockaddr_in Addr;
			Addr.sin_family = AF_INET;
			Addr.sin_port = htons(m_Port);
			Addr.sin_addr.s_addr = ADDR_ANY;
			int32_t rc = bind(pSock, (sockaddr*)&Addr, sizeof(SOCKADDR_IN));

			if (rc == SOCKET_ERROR) throw CForgeExcept("Binding socket failed. WSA error: " + std::to_string(WSAGetLastError()));
			rc = listen(pSock, 250);
			if (rc == SOCKET_ERROR) throw CForgeExcept("Setting listen of TCP server failed: " + std::to_string(WSAGetLastError()));
		}

		m_pHandle = (void*)pSock;

		// create Buffer
		m_BufferSize = 2048;
		m_pInBuffer = new uint8_t[m_BufferSize];
		m_pOutBuffer = new uint8_t[m_BufferSize];

		// start threads
		if (Type == TYPE_SERVER) {
			m_pAcceptThread = new std::thread(&TCPSocket::acceptThread, this);
		}

	}//begin

	void TCPSocket::end(void) {
		// close socket
		if (m_pHandle != nullptr) closesocket((SOCKET)m_pHandle);
		m_pHandle = nullptr;
		if (m_pAcceptThread != nullptr) m_pAcceptThread->join();
		m_pAcceptThread = nullptr;

		// close all connections
		for (auto &i : m_ActiveConnections) {
			if (nullptr == i) continue;
			closesocket((SOCKET)i->pHandle);
			i->pHandle = nullptr;
			i->pRecvThread->join();
			delete i;
		}
		m_ActiveConnections.clear();

		if (nullptr != m_pInBuffer) delete[] m_pInBuffer;
		if (nullptr != m_pOutBuffer) delete[] m_pOutBuffer;
		m_BufferSize = 0;

		m_pInBuffer = nullptr;
		m_pOutBuffer = nullptr;
	}//end

	void TCPSocket::sendData(uint8_t* pData, uint32_t DataSize, int32_t ConnectionID) {
		if (nullptr == m_pHandle) throw CForgeExcept("Socket not valid!");
		if (ConnectionID >= m_ActiveConnections.size()) throw IndexOutOfBoundsExcept("ConnectionID");
		if (nullptr == m_ActiveConnections[ConnectionID]) throw CForgeExcept("Connection " + std::to_string(ConnectionID) + " not valid!");

		SOCKET pSock = (SOCKET)m_pHandle;
		Connection* pCon = m_ActiveConnections[ConnectionID];
		send((SOCKET)pCon->pHandle, (const char*)pData, DataSize, 0);

	}//send

	bool TCPSocket::recvData(uint8_t* pBuffer, uint32_t* pDataSize, int32_t ConnectionID){
		if (ConnectionID >= m_ActiveConnections.size()) throw IndexOutOfBoundsExcept("ConnectionID");
		if (nullptr == m_ActiveConnections[ConnectionID]) throw CForgeExcept("Invalid connection with id " + std::to_string(ConnectionID));

		Connection* pCon = m_ActiveConnections[ConnectionID];

		if (pCon->InQueue.empty()) return false;

		pCon->Mutex.lock();
		Package* pRval = pCon->InQueue.front();
		pCon->InQueue.pop();
		pCon->Mutex.unlock();
		(*pDataSize) = pRval->DataSize;
		memcpy(pBuffer, pRval->pData, pRval->DataSize);
		delete pRval;
		return true;
	}//getMessage


	bool TCPSocket::connectTo(std::string IP, uint16_t Port) {
		bool Rval = false;

		SOCKET pSock = (SOCKET)m_pHandle;

		SOCKADDR_IN Addr;
		Addr.sin_family = AF_INET;
		Addr.sin_addr.s_addr = inet_addr(IP.c_str());
		Addr.sin_port = htons(Port == 0 ? m_Port : Port);

		auto Rc = connect(pSock, (SOCKADDR*)&Addr, sizeof(SOCKADDR));
		if (Rc == SOCKET_ERROR) throw CForgeExcept("Connecting to " + IP + "failed!");

		Connection* pCon = new Connection();
		pCon->IP = IP;
		pCon->Port = Port;
		pCon->pHandle = (void*)pSock;
		pCon->pRecvThread = new std::thread(&TCPSocket::recvThread, this, int32_t(m_ActiveConnections.size()) );
		m_ActiveConnections.push_back(pCon);

		Rval = true;

		return Rval;
	}//connect

	uint32_t TCPSocket::activeConnections(void)const {
		return uint32_t(m_ActiveConnections.size());
	}//activeConnections

	void TCPSocket::acceptThread(void) {
		SOCKET pSock = (SOCKET)m_pHandle;

		while (nullptr != m_pHandle) {
			sockaddr_in AddrIn;
			int32_t AddrLen = sizeof(AddrIn);
			memset(&AddrIn, 0, sizeof(AddrIn));

			SOCKET pS = accept(pSock, (SOCKADDR*)&AddrIn, &AddrLen);
			if (pS != SOCKET_ERROR) {

				Connection* pNewConnection = new Connection();
				int32_t ConnectionID = m_ActiveConnections.size();
				m_ActiveConnections.push_back(pNewConnection);
				pNewConnection->pHandle = (void*)pS;
				pNewConnection->IP = inet_ntoa(AddrIn.sin_addr);
				pNewConnection->Port = ntohs(AddrIn.sin_port);
				pNewConnection->pRecvThread = new std::thread(&TCPSocket::recvThread, this, ConnectionID);	
			}
		}//while[do not leave thread]
	}//socketThread

	void TCPSocket::recvThread(int32_t ConnectionID) {
		Connection* pCon = m_ActiveConnections[ConnectionID];

		SOCKET pSock = (SOCKET)pCon->pHandle;

		while (pCon->pHandle != nullptr) {

			sockaddr_in AddrIn;
			int32_t AddrLen;

			int32_t MsgLength = recv(pSock, (char*)m_pInBuffer, m_BufferSize, 0);
			if (MsgLength > 0) {
				pCon->Mutex.lock();
				Package* pP = new Package();
				pP->pData = new uint8_t[MsgLength];
				memcpy(pP->pData, m_pInBuffer, MsgLength);
				pP->DataSize = MsgLength;
				pCon->InQueue.push(pP);
				pCon->Mutex.unlock();
			}
		}//while[do not leave thread]

	}//socketClientThread

}//name space