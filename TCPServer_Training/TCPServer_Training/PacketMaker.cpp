#include "PacketMaker.h"
#include <iostream>


// About UserData
UserData::UserData() : UserSocket(INVALID_SOCKET)
{
}

UserData::UserData(const string& id) : UserSocket(INVALID_SOCKET)
{
	UserName = id;
}

UserData::~UserData()
{
	if (UserSocket != INVALID_SOCKET)
	{
		closesocket(UserSocket);
	}
}

// About PacketMaker
pair<char*, int> PacketMaker::MakeBuffer(EPacket value)
{
	int bufferSize = DefaultBufferSize;
	char* buffer = new char[bufferSize];

	MakeHeader(buffer, value, 0);

	return make_pair(buffer, bufferSize);
}

pair<char*, int> PacketMaker::MakeBuffer(EPacket value, const char* data)
{
	int packetSize = (int)strlen(data);
	int bufferSize = DefaultBufferSize + packetSize;

	char* buffer = new char[bufferSize];

	MakeHeader(buffer, value, packetSize);

	memcpy(&buffer[4], data, packetSize);

	return make_pair(buffer, bufferSize);
}

char* PacketMaker::MakeHeader(char* buffer, EPacket value, unsigned short size)
{
	// Header
	//size code
	//[][] [][]
	unsigned short newSize = htons(size + 2);
	unsigned short code = htons(static_cast<unsigned short>(value));

	memcpy(buffer, &newSize, 2);
	memcpy(&buffer[2], &code, 2);

	return buffer;
}

bool PacketMaker::SendPacket(SOCKET* clientSocket, const EPacket& value)
{
	pair<char*, int> bufferData = MakeBuffer(value);

	int sendByte = send(*clientSocket, bufferData.first, bufferData.second, 0);
	if (sendByte <= 0)
	{
		cout << "Send Error : " << GetLastError() << endl;
		return false;
	}
	return true;
}

bool PacketMaker::SendPacket(SOCKET* clientSocket, const EPacket& value, const char* message)
{
	pair<char*, int> bufferData = MakeBuffer(value, message);

	int sendByte = send(*clientSocket, bufferData.first, bufferData.second, 0);
	if (sendByte <= 0)
	{
		cout << "Send Error : " << GetLastError() << endl;
		return false;
	}
	return true;
}

void PacketMaker::SendPacketAllClient(const map<unsigned short, UserData>& list, EPacket value, const char* message)
{
	pair<char*, int> bufferData = MakeBuffer(value, message);

	for (const auto& user : list)
	{
		if (user.second.UserSocket == INVALID_SOCKET)
		{
			continue;
		}

		int sendByte = send(user.second.UserSocket, bufferData.first, bufferData.second, 0);
		if (sendByte <= 0)
		{
			cout << "Send Error : " << user.first << ". ErrorCode : " << GetLastError() << endl;
			continue;
		}
	}
}

void PacketMaker::SendPacketAllClient(const map<unsigned short, UserData>& list, EPacket value, const char* message, unsigned short userNum)
{
	pair<char*, int> bufferData = MakeBuffer(value, message);

	for (const auto& user : list)
	{
		if (user.second.UserSocket == INVALID_SOCKET)
		{
			continue;
		}

		if (user.first != userNum)
		{
			int SendByte = send(user.second.UserSocket, bufferData.first, bufferData.second, 0);
			if (SendByte <= 0)
			{
				cout << "Send Error : " << user.first << ". ErrorCode : " << GetLastError() << endl;
				continue;
			}
		}
	}
}
