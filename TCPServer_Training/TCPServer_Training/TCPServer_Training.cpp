#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

#pragma comment(lib,"ws2_32.lib")
#define PACKET_SIZE 1024

SOCKET ServerSocket, ClientSocket;

void proc_recvs();

int main()
{
	WSADATA WsaData;

	int Result = WSAStartup(MAKEWORD(2, 2), &WsaData);

	if (Result != 0)
	{
		cout << "Error" << endl;
		exit(-1);
	}

	ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ServerSocket == INVALID_SOCKET)
	{
		cout << "Socket Error\n";
		cout << "Socket Error Number : " << GetLastError() << endl;
		exit(-1);
	}

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 나중에 서버 주소 바꿀 것
	ServerSockAddr.sin_port = htons(7777); // port 번호도 바꿔

	Result = bind(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	if (Result != 0)
	{
		cout << "Socket Error" << endl;
		cout << "Socket Error Number : " << GetLastError() << endl;
		exit(-1);
	}

	Result = listen(ServerSocket, SOMAXCONN);

	if (Result == SOCKET_ERROR)
	{
		cout << "Listen Error" << endl;
		cout << "Socket Error Number : " << GetLastError() << endl;
		exit(-1);
	}

	while (true)
	{
		SOCKADDR_IN ClientSock;
		ZeroMemory(&ClientSock, sizeof(ClientSock));
		int ClientSockLength = sizeof(ClientSock);

		ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientSock, &ClientSockLength);

		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "Accept Error" << endl;
			cout << "Socket Error Number : " << GetLastError() << endl;
			exit(-1);
		}
		
		char Message[1024] = { 0, };

		cout << "Send to ";
		cin >> Message;

		int SendByte = send(ClientSocket, Message, 9, 0);

		if (SendByte <= 0)
		{
			cout << "Send Error" << endl;
			cout << "Socket Error Number : " << GetLastError() << endl;
			exit(-1);
		}
		
		char Recv[1024] = { 0, };

		int RecvByte = recv(ClientSocket, Recv, 4, MSG_WAITALL);

		if (RecvByte <= 0)
		{
			cout << "Recv Error" << endl;
			cout << "Socket Error Number : " << GetLastError() << endl;
			exit(-1);
		}

		cout << "Recv to Client : " << Recv << endl << endl;
	}

	closesocket(ClientSocket);
	closesocket(ServerSocket);
	WSACleanup();

	return 0;
}

void proc_recvs()
{
	char buffer[PACKET_SIZE] = { 0 };

	while (!WSAGetLastError()) { //여긴 일부로 차이점을 일부러 exit나 이런 이벤트 안넣었습니다
		ZeroMemory(&buffer, PACKET_SIZE);
		recv(ClientSocket, buffer, PACKET_SIZE, 0);
		cout << "받은 메세지: " << buffer << endl;
	}
}
