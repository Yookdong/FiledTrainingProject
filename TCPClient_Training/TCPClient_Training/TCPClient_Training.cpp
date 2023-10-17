#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <process.h>
using namespace std;

#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32")
#define PACKET_SIZE 1024

SOCKET ServerSocket;

void proc_recv();

int main()
{
	WSAData wsaData;

	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (Result != 0)
	{
		cout << "Error" << endl;
		exit(-1);
	}

	while (true)
	{
		ServerSocket = socket(PF_INET, SOCK_STREAM, 0);

		SOCKADDR_IN ServerSock;
		ZeroMemory(&ServerSock, 0);
		ServerSock.sin_family = PF_INET;
		ServerSock.sin_port = htons(7777);
		inet_pton(AF_INET,
			"127.0.0.1", &(ServerSock.sin_addr.s_addr));

		Result = connect(ServerSocket, (SOCKADDR*)&ServerSock, sizeof(ServerSock));

		if (Result == SOCKET_ERROR)
		{
			cout << "Connect Error" << endl;
			cout << "Socket Error Number : " << GetLastError() << endl;
			closesocket(ServerSocket);
			continue;
		}

		// 서버에서 메세지 받기
		char Recv[PACKET_SIZE] = { 0, };

		int RecvByte = recv(ServerSocket, Recv, sizeof(Recv), 0);

		if (RecvByte <= 0)
		{
			cout << "Recv Error" << endl;
			cout << "Socket Error Number : " << GetLastError() << endl;
			closesocket(ServerSocket);
			continue;
		}

		cout << "Receive Message : " << Recv << endl;

		// 서버로 메세지 보내기
		char Message[PACKET_SIZE] = { 0, };

		cout << "Send to ";
		cin >> Message;

		int SendSize = send(ServerSocket, Message, sizeof(Message), 0);

		if (SendSize <= 0)
		{
			cout << "Send Error" << endl;
			cout << "Socket Error Number : " << GetLastError() << endl;
			closesocket(ServerSocket);
			continue;
		}
	}

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}

void proc_recv()
{
	char buffer[PACKET_SIZE] = { 0, }; //char 생성
	string str; //string 생성
	
	while (!WSAGetLastError())
	{
		ZeroMemory(&buffer, PACKET_SIZE); //buffer 비우기
		recv(ServerSocket, buffer, PACKET_SIZE, 0); //데이터받아오기
		str = buffer; //buffer의값이 str에 들어갑니다
		if (str == "hi") break; //str의값이 "exit"일경우 데이터받아오기'만' 종료
		cout << "받은 메세지: " << buffer << endl;
	}
}
