#pragma once

#include "Packet.h"
#include <utility>
#include <WinSock2.h>
#include <string>
#include <map>

using namespace std;

class UserData
{
public:
	SOCKET UserSocket;
	string UserName;
	string UserPassward;

public:
	UserData();
	UserData(const string& id);
	~UserData();

	//// Getter
	//SOCKET* GetUserSocket() { return &UserSocket; }
	//string GetUserName() { return UserName; }
	//string GetUserPassward() { return UserPassward; }

	//// Setter
	//void SetUserSocket(SOCKET& socket) { UserSocket = socket; }
	//void SetUserName(string name) { UserName = name; }
	//void SetUserPassward(string pw) { UserPassward = pw; }
};

class PacketMaker
{
protected:
	static const int DefaultBufferSize = 4;

	static pair<char*, int> MakeBuffer(EPacket value);
	static pair<char*, int> MakeBuffer(EPacket value, const char* data);

	static char* MakeHeader(char* buffer, EPacket value, unsigned short size);

public:
	static bool SendPacket(SOCKET* clientSocket, const EPacket& value);
	static bool SendPacket(SOCKET* clientSocket, const EPacket& value, const char* message);

	static void SendPacketAllClient(const map<unsigned short, UserData>& list,EPacket value, const char* message);
	static void SendPacketAllClient(const map<unsigned short, UserData>& list, EPacket value, const char* message, unsigned short userNum);
};