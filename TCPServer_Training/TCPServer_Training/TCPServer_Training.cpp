#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

#pragma comment(lib,"ws2_32.lib")

int main()
{
	WSADATA WsaData;

	int Result = WSAStartup(MAKEWORD(2, 2), &WsaData);

	if (Result != 0)
	{
		cout << "Error" << endl;
	}

	SOCKET ServerSocket;
	ServerSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (ServerSocket == INVALID_SOCKET)
	{
		cout << "Socket Error\n";
		cout << "Socket Error Number : " << GetLastError() << endl;
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
	}

	Result = listen(ServerSocket, SOMAXCONN);

	if (Result == SOCKET_ERROR)
	{
		cout << "Listen Error" << endl;
		cout << "Socket Error Number : " << GetLastError() << endl;
	}

	while (true)
	{
		SOCKADDR_IN ClientSock;
		ZeroMemory(&ClientSock, sizeof(ClientSock));
		int ClientSockLength = sizeof(ClientSock);

		SOCKET ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientSock, &ClientSockLength);

		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "Accept Error" << endl;
			cout << "Socket Error Number : " << GetLastError() << endl;
		}
		
		char Message[1024] = { 0, };

		cout << "Send to " << Message << endl; // 보낼 메세지 만들기

		int SendByte = send(ClientSocket, Message, 9, 0);

		if (SendByte <= 0)
		{
			cout << "Send Error" << endl;
			cout << "Socket Error Number : " << GetLastError() << endl;
		}
		
		char Recv[1024] = { 0, };

		int RecvByte = recv(ClientSocket, Recv, 4, MSG_WAITALL);

		if (RecvByte <= 0)
		{
			cout << "Recv Error" << endl;
			cout << "Socket Error Number : " << GetLastError() << endl;
		}

		cout << "Recv to Client : " << *(int*)&Recv << endl << endl;

		closesocket(ClientSocket);
	}

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}