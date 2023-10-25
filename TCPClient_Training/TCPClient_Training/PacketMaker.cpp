#include "PacketMaker.h"
#include <iostream>

pair<char*, int> PacketMaker::MakeBuffer(EPacket value)
{
	int bufferSize = DefaultBufferSize;
	char* buffer = new char[bufferSize];

	MakeHeader(buffer, value, 0);

	return make_pair(buffer, bufferSize);
}

pair<char*, int> PacketMaker::MakeBuffer(EPacket value, const char* data)
{
	// Header  Data
	//[][][][] [Variable data]

	int packetSize = (int)strlen(data);
	int bufferSize = DefaultBufferSize + packetSize;

	char* buffer = new char[bufferSize];
	buffer = MakeHeader(buffer, value, packetSize);

	memcpy(&buffer[4], data, packetSize);

	return make_pair(buffer, bufferSize);
}

char* PacketMaker::MakeHeader(char* buffer, EPacket value, unsigned short size)
{
	//size   code  
	//[][]   [][]
	unsigned short newsize = htons(size + 2);
	unsigned short code = htons(static_cast<unsigned short>(value));

	memcpy(buffer, &newsize, 2);
	memcpy(&buffer[2], &code, 2);

	return buffer;
}

bool PacketMaker::SendPacket(SOCKET* serverSocket, const EPacket& packet)
{
	pair<char*, int> bufferData = MakeBuffer(packet);

	int sendByte = send(*serverSocket, bufferData.first, bufferData.second, 0);
	if (sendByte <= 0)
	{
		cout << "Send Error" << GetLastError() << endl;
		return false;
	}

	return true;
}

bool PacketMaker::SendPacket(SOCKET* serverSocket, const EPacket& packet, const char* message)
{
	pair<char*, int> bufferData = MakeBuffer(packet, message);

	int sendByte = send(*serverSocket, bufferData.first, bufferData.second, 0);
	if (sendByte <= 0)
	{
		cout << "Send Error" << GetLastError() << endl;
		return false;
	}
	 
	return true;
}