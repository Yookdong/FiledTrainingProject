#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <process.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

#include "PacketMaker.h"

using namespace std;
#pragma comment(lib, "ws2_32")
#define PACKET_SIZE_MAX 1024

EPacket CurrentPacket = EPacket::None;

unsigned WINAPI RecvThread(void* arg);
unsigned WINAPI SendThread(void* arg);

void ChatProgramStart(SOCKET& socket);
void SendCheckMessage(SOCKET& socket, const EPacket& value);
void CheckID(SOCKET& socket, const EPacket& value);
void CheckPW(SOCKET& socket, const EPacket& value);
void SendChating(SOCKET& socket);

bool bCheckSendSuccess = false;

int main()
{
	WSAData wsaData;

	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (Result != 0)
	{
		cout << "WSAData Error" << endl;
		exit(-1);
	}

	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (ServerSocket == SOCKET_ERROR)
	{
		cout << "ServerSocket Error Num : " << GetLastError() << endl;
		exit(-1);
	}

	SOCKADDR_IN ServerSock;
	ZeroMemory(&ServerSock, 0);
	ServerSock.sin_family = PF_INET;
	ServerSock.sin_port = htons(7777);
	inet_pton(AF_INET,
		"127.0.0.1", &(ServerSock.sin_addr.s_addr));

	Result = connect(ServerSocket, (SOCKADDR*)&ServerSock, sizeof(ServerSock));

	if (Result == SOCKET_ERROR)
	{
		cout << "Connect Error Num : " << GetLastError() << endl;
		exit(-1);
	}
	else
	{
		cout << "Server Connect\n-----\n\n";
	}

	HANDLE ThreadHandles[2];
	ThreadHandles[0] = (HANDLE)_beginthreadex(nullptr, 0, RecvThread, (void*)&ServerSocket, 0, nullptr);
	ThreadHandles[1] = (HANDLE)_beginthreadex(nullptr, 0, SendThread, (void*)&ServerSocket, 0, nullptr);

	WaitForMultipleObjects(2, ThreadHandles, TRUE, INFINITE);

	CloseHandle(ThreadHandles[1]);
	CloseHandle(ThreadHandles[0]);
	closesocket(ServerSocket);
	WSACleanup();

	return 0;
}

unsigned __stdcall RecvThread(void* arg)
{
	// 여기에 자료를 받은 자료를 분석하는 코드를 넣으면 된다.
	printf("Start RecvThread\n\n");

	SOCKET serverSocket = *(SOCKET*)arg;

	while (true)
	{
		// 받은 자료에 따라서 아래 코드는 수정을 해야 한다.

		// 일단 받은 데이터 전체를 가지고 하는 코드
		// 패킷을 이제 만들어서 어떻게 자료를 가지고 갈지 정하면 된다. 
		unsigned short packetSize = 0;
		int recvByte = recv(serverSocket, (char*)(&packetSize), 2, MSG_WAITALL);

		if (recvByte <= 0)
		{
			cout << "Recv Error Num : " << GetLastError() << endl;
			closesocket(serverSocket);
			break;
		}
		cout << "recv packetSize : " << packetSize << endl;
		packetSize = ntohs(packetSize);

		char* buffer = new char[packetSize];

		recvByte = recv(serverSocket, buffer, packetSize, MSG_WAITALL);

		if (recvByte <= 0)
		{
			cout << "Recv Error Num : " << GetLastError() << endl;
			closesocket(serverSocket);
			break;
		}
		cout << "recv buffer : " << buffer << endl;
		unsigned short code = 0;
		memcpy(&code, buffer, 2);
		EPacket ECode = (EPacket)(ntohs(code));

		switch (ECode)
		{
		case EPacket::None:
			cout << "Send to Server\n";
			break;
		case EPacket::ProgramStart:
			CurrentPacket = EPacket::ProgramStart;
			break;
		case EPacket::S2C_IsLogin:
			cout << "Input Your ID : ";
			CurrentPacket = EPacket::C2S_ReqCheckID;
			break;
		case EPacket::S2C_IsJoin:
			cout << "Input ID you want to use : ";
			CurrentPacket = EPacket::C2S_Make_NewID;
			break;
		case EPacket::S2C_ID_Success:
			cout << "Input Your PW : ";
			CurrentPacket = EPacket::C2S_ReqCheckPW;
			break;
		case EPacket::S2C_ID_Failed:
			cout << "Retry Input Your ID : ";
			CurrentPacket = EPacket::C2S_ReqCheckID;
			break;
		case EPacket::S2C_PW_Success:
			system("cls");
			cout << "find other User\n";
			cout << "Please Wait\n";
			cout << "---------------------------------------------------------\n\n";
			CurrentPacket = EPacket::C2S_IsCanChat;
			break;
		case EPacket::S2C_PW_Failed:
			cout << "Retry Input Your PW : ";
			CurrentPacket = EPacket::C2S_ReqCheckPW;
			break;
		case EPacket::S2C_Make_ID_Success:
			cout << "Input PW you want to use : ";
			CurrentPacket = EPacket::C2S_Make_NewPW;
			break;
		case EPacket::S2C_Make_ID_Failed:
			cout << "Retry Input ID you want to use : ";
			CurrentPacket = EPacket::C2S_Make_NewID;
			break;
		case EPacket::S2C_Make_PW_Success:
			cout << "Join Success\n";
			cout << "Try Login Now\n";
			cout << "---------------------------------------------------------\n\n";
			CurrentPacket = EPacket::C2S_Join_Success;
			cout << "????\n";
			break;
		case EPacket::S2C_Make_PW_Failed:
			cout << "Retry Input PW you want to use : ";
			CurrentPacket = EPacket::C2S_Make_NewPW;
			break;
		case EPacket::S2C_CanChat:
			cout << "You can start Chating!\n";
			CurrentPacket = EPacket::C2S_Chat;
			break;
		case EPacket::S2C_Chat:
		{
			char message[PACKET_SIZE_MAX] = { 0, };
			memcpy(&message, buffer + 2, packetSize - 2);
			cout << message << endl;
		}
		break;
		case EPacket::Max:
			cout << "Server Error\n";
			exit(-1);
			break;
		default:
			break;
		}
	}

	return 0;
}

unsigned __stdcall SendThread(void* arg)
{
	printf("Start SendThread\n\n");

	SOCKET serverSocket = *(SOCKET*)arg;

	while (true)
	{
		switch (CurrentPacket)
		{
		case EPacket::None:
			break;
		case EPacket::ProgramStart:
			ChatProgramStart(serverSocket);
			break;
		case EPacket::C2S_Join_Success:
			cout << "tesst\n";
			PacketMaker::SendPacket(&serverSocket, EPacket::C2S_IsLogin);
			break;
		case EPacket::C2S_IsCanChat:
			// 지금 대화할 상대가 있는지 물어본다.
			break;
		case EPacket::C2S_Chat:
			SendChating(serverSocket);
			break;
		default:
			SendCheckMessage(serverSocket, CurrentPacket);
			break;
		}
	}

	return 0;
}

void ChatProgramStart(SOCKET& socket)
{
	cout << "Hello! This Chating Program\n";

	while (true)
	{
		cout << "What do you want to do?\n";
		cout << "1. Login     2. Join\n";

		char check[10] = { 0, };
		cin >> check;
		cin.ignore();

		if (strlen(check) > 1)
		{
			cout << "Input Error\n";
			cout << "Please Retry\n\n";
			continue;
		}

		switch (check[0])
		{
		case '1':
			bCheckSendSuccess = PacketMaker::SendPacket(&socket, EPacket::C2S_IsLogin);
			if (!bCheckSendSuccess)
			{
				cout << "Send Error\n";
			}
			else
			{
				//CurrentPacket = EPacket::None;
			}
			break;
		case '2':
			bCheckSendSuccess = PacketMaker::SendPacket(&socket, EPacket::C2S_IsJoin);
			if (!bCheckSendSuccess)
			{
				cout << "Send Error\n";
			}
			else
			{
				//CurrentPacket = EPacket::None;
			}
			break;
		default:
			cout << "Input Error\n";
			cout << "Please Retry\n\n";
			continue;
		}

		break;
	}
}

void SendCheckMessage(SOCKET& socket, const EPacket& value)
{
	switch (value)
	{
	case EPacket::C2S_ReqCheckID:
		CheckID(socket, value);
		break;
	case EPacket::C2S_ReqCheckPW:
		CheckPW(socket, value);
		break;
	case EPacket::C2S_Make_NewID:
		CheckID(socket, value);
		break;
	case EPacket::C2S_Make_NewPW:
		CheckPW(socket, value);
		break;
	default:
		break;
	}
}

void CheckID(SOCKET& socket, const EPacket& value)
{
	char userID[10] = { 0, };

	while (true)
	{
		cin >> userID;
		cin.ignore();
		if (strlen(userID) > 100)
		{
			cout << "ID too Long\n";
			cout << "Retry Input Your ID\n";
			continue;
		}

		bCheckSendSuccess = PacketMaker::SendPacket(&socket, value, userID);
		if (!bCheckSendSuccess)
		{
			cout << "Send Error\n";
		}
	}
}

void CheckPW(SOCKET& socket, const EPacket& value)
{
	char userPW[10] = { 0, };

	while (true)
	{
		cin >> userPW;
		cin.ignore();
		if (strlen(userPW) > 100)
		{
			cout << "PW too Long\n";
			cout << "Retry Input Your PW\n";
			continue;
		}

		bCheckSendSuccess = PacketMaker::SendPacket(&socket, value, userPW);
		if (!bCheckSendSuccess)
		{
			cout << "Send Error\n";
		}
	}
}

void SendChating(SOCKET& socket)
{
	char message[PACKET_SIZE_MAX] = { 0, };

	while (true)
	{
		cin >> message;
		cin.ignore();
		if (strlen(message) > (PACKET_SIZE_MAX - 1))
		{
			cout << "Message too Long\n";
			cout << "Retry Input Your PW\n";
			continue;
		}

		bCheckSendSuccess = PacketMaker::SendPacket(&socket, EPacket::C2S_Chat, message);
		if (!bCheckSendSuccess)
		{
			cout << "Send Error\n";
		}
	}
}
