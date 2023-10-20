#pragma once

#include "Packet.h"
#include <utility>
#include <WinSock2.h>

using namespace std;

class PacketMaker
{
protected:
	static const int DefaultBufferSize = 4;

	static pair<char*, int> MakeBuffer(EPacket value);
	static pair<char*, int> MakeBuffer(EPacket value, const char* data);

	static char* MakeHeader(char* Buffer, EPacket Type, unsigned short Size);

public:
	static bool SendPacket(SOCKET* serverSocket, const EPacket& packet);
	static bool SendPacket(SOCKET* serverSocket, const EPacket& packet, const char* message);
};

