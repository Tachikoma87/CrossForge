#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/Internet/TCPSocket.h"
#include "InternetFileStream.h"





namespace CForge {

	void InternetFileStream::sendFile(const char* path,uint32_t packageSize, TCPSocket sender,int32_t connection)
	{
		File send_file;
		send_file.begin(path, "rb");
		int size = send_file.size(path);
		uint8_t* pBuffer = (uint8_t*)malloc(size);
		uint32_t bytes = size;
		send_file.read(pBuffer, bytes);
		uint8_t ack[1];
		uint32_t ackSize = 1;
		std::string temp = std::to_string(bytes);
		const char* bytes_char = temp.c_str();
		uint8_t* filesize = (uint8_t*)malloc(sizeof(uint8_t) * temp.size());
		//write size of file into buffer
		int index = 0;
		for (int i = temp.size() - 1; i >= 0; i--) {
			filesize[index] = bytes_char[i];
			filesize[index + 1] = 'F';
			index++;
		}
		//send Filesize
		sender.sendData(filesize, (uint32_t)sizeof(temp) + 1, connection);
		int package = 0;
		int package_count = (size / packageSize) + 1;
		//send FileData
		while (package < package_count)
		{

			while (sender.recvData(ack, &ackSize, connection) && package < package_count)
			{
				size = size - packageSize;
				if (bytes < 0) packageSize = packageSize + bytes;
				sender.sendData(pBuffer + package * 128, packageSize, connection);//
				package++;
			}
		}
		//wait for last acknowledge
	while(!(sender.recvData(ack, &ackSize, connection))){}
	return;
	}



	void InternetFileStream::receiveFile(const char* path, TCPSocket receiver, int32_t connection, uint8_t* recvBuffer, uint32_t DataSize)
	{
		uint8_t ack[1];
		uint8_t ackSize = 1;
		//start file to write into
		File s_file;
		s_file.begin(path, "wb");

		int size = 0;
		int x = 0;
		//get size of file from buffer 
		while (recvBuffer[x] != 'F' || x > DataSize) {					//stop when flag or length of buffer reached
			size = size + (int)(recvBuffer[x] - '0') * pow(10, x);
			x++;
		}

		int package = 0;
		//send first acknowledge
		receiver.sendData(ack, ackSize, connection);

		int received = 0;
		//wait for all packages from client
		while ((size - received) > 0) {
			while (receiver.recvData(recvBuffer, &DataSize, connection) && (size - received) > 0)
			{
				int o = 0;
				//discard additional buffer
				if (size - received < DataSize) o = DataSize - (size - received);
				//write buffer into file
				s_file.write(recvBuffer, DataSize-o);
				received += DataSize;
				//send acknowledge to get next package
				receiver.sendData(ack, ackSize, connection);
			}
		}

		//writing into file finished -> close file
		s_file.end();
		return;
	}

}