#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <process.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

using namespace std;
#pragma comment(lib, "ws2_32")
#define PACKET_SIZE 1024

unsigned WINAPI RecvThread(void* arg);
unsigned WINAPI SendThread(void* arg);

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
		cout << "Server Connect\n";
	}

	//// 서버에서 메세지 받기
	//char Recv[PACKET_SIZE] = { 0, };

	//int RecvByte = recv(ServerSocket, Recv, sizeof(Recv), 0);

	//if (RecvByte <= 0)
	//{
	//	cout << "Recv Error" << endl;
	//	cout << "Socket Error Number : " << GetLastError() << endl;
	//	closesocket(ServerSocket);
	//	continue;
	//}

	//cout << "Receive Message : " << Recv << endl;

	//// 서버로 메세지 보내기
	//char Message[PACKET_SIZE] = { 0, };

	//cout << "Send to ";
	//cin >> Message;

	//int SendSize = send(ServerSocket, Message, sizeof(Message), 0);

	//if (SendSize <= 0)
	//{
	//	cout << "Send Error" << endl;
	//	cout << "Socket Error Number : " << GetLastError() << endl;
	//	closesocket(ServerSocket);
	//	continue;
	//}

	//closesocket(ServerSocket);

	HANDLE ThreadHandles[2];
	ThreadHandles[0] = (HANDLE)_beginthreadex(nullptr, 0, RecvThread, (void*)&ServerSocket, 0, nullptr);
	ThreadHandles[1] = (HANDLE)_beginthreadex(nullptr, 0, SendThread, (void*)&ServerSocket, 0, nullptr);

	WaitForMultipleObjects(2, ThreadHandles, TRUE, INFINITE);

	WSACleanup();

	return 0;
}

unsigned __stdcall RecvThread(void* arg)
{
	// 여기에 자료를 받은 자료를 분석하는 코드를 넣으면 된다.

	SOCKET serverSocket = *(SOCKET*)arg;

	while (true)
	{
		// 받은 자료에 따라서 아래 코드는 수정을 해야 한다.

		// 일단 받은 데이터 전체를 가지고 하는 코드
		// 패킷을 이제 만들어서 어떻게 자료를 가지고 갈지 정하면 된다. 
		char buffer[PACKET_SIZE] = { 0, };

		int recvByte = recv(serverSocket, buffer, PACKET_SIZE, MSG_WAITALL);

		if (recvByte <= 0)
		{
			cout << "Recv Error Num : " << GetLastError() << endl;
			closesocket(serverSocket);
			break;
		}

		// 받은 버퍼를 가지고 할 수 있는 코드를 짠다.
		cout << buffer << endl;
	}

	return 0;
}

unsigned __stdcall SendThread(void* arg)
{
	SOCKET serverSocket = *(SOCKET*)arg;

	while (true)
	{
		char message[PACKET_SIZE] = { 0, };

		cin >> message;
		cin.ignore();

		int sendByte = send(serverSocket,message, (int)strlen(message), 0);
		if (sendByte <= 0)
		{
			cout << "Send Error Num : " << GetLastError() << endl;
			closesocket(serverSocket);
			break;
		}

	}
	return 0;
}
