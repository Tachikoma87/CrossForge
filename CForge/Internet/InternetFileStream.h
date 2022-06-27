




#include <thread>
#include <queue>
#include <mutex>
#include <memory>
#include "../Core/CForgeObject.h"


namespace CForge {
	class InternetFileStream : public CForgeObject {


	public:
		static void sendFile(const char* path, uint32_t packageSize, TCPSocket sender, int32_t connection);
		static void receiveFile(const char* path, TCPSocket receiver, int32_t connection, uint8_t* recvBuffer, uint32_t DataSize);

	};
}