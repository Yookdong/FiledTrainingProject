#include "PacketMaker.h"

pair<char*, int> PacketMaker::MakeBuffer(EPacket value)
{
    return pair<char*, int>();
}

pair<char*, int> PacketMaker::MakeBuffer(EPacket value, const char* data)
{
    return pair<char*, int>();
}

char* PacketMaker::MakeHeader(char* buffer, EPacket type, unsigned short size)
{
    return nullptr;
}

bool PacketMaker::SendPacket(SOCKET* clientSocket, const EPacket& packet)
{
    return false;
}

bool PacketMaker::SendPacket(SOCKET* clientSocket, const EPacket& packet, const char* message)
{
    return false;
}
