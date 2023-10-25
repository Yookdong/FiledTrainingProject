#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include "PacketMaker.h"

#pragma comment(lib,"ws2_32.lib")
#define PACKET_SIZE_MAX 1024
using namespace std;

//----- MySQL -----
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

// 실험용 나중에 수정 필요함
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "ehggur0347!";

sql::Driver* SQL_Driver;
sql::Connection* SQL_Connection;
sql::Statement* SQL_State;
sql::ResultSet* SQL_Result;
//----------

fd_set Reads; // 원본
fd_set CopyReads; // 복사본

map<unsigned short, UserData> JoinUserList;
map<unsigned short, UserData> LoginUserList;

unsigned WINAPI Thread_Server(void* arg);
unsigned WINAPI RecvThread(void* arg);
unsigned WINAPI SendThread(void* arg);
void SendMessage(SOCKET& socket, const char* str);
void RecvMessage(SOCKET& socket, char* str);
void SendError(SOCKET& socket);
void RecvError(SOCKET& socket);

int main()
{
	//----- DB 관련 코드 생성 -----
	try
	{
		SQL_Driver = get_driver_instance();
		SQL_Connection = SQL_Driver->connect(server, username, password); // 서버 연결
		cout << "DB Server Connect Success\n";
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	// 미리 생성해 둔 스키마 연결
	SQL_Connection->setSchema("dbserver_tcp");
	//----------

	cout << "Server Start\n";

	WSADATA WsaData;

	int Result = WSAStartup(MAKEWORD(2, 2), &WsaData);

	if (Result != 0)
	{
		cout << "WSADATA Error" << endl;
		exit(-1);
	}

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "ListenSocket Error Num : " << GetLastError() << endl;
		exit(-1);
	}

	SOCKADDR_IN ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = AF_INET;
	ListenSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 나중에 서버 주소 바꿀 것
	ListenSockAddr.sin_port = htons(7777); // port 번호도 바꿔

	Result = _WINSOCK2API_::bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	if (Result != 0)
	{
		cout << "Bind Error Number : " << GetLastError() << endl;
		exit(-1);
	}

	Result = listen(ListenSocket, SOMAXCONN);

	if (Result == SOCKET_ERROR)
	{
		cout << "Listen Error Number : " << GetLastError() << endl;
		exit(-1);
	}

	// Thread 용
	struct timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 500;

	FD_ZERO(&Reads);
	FD_SET(ListenSocket, &Reads);

	printf("Wait for Connecting\n");

	while (true)
	{
		// 시작 시 항상 원본을 복사본으로 해놔야 비교가 가능
		CopyReads = Reads;

		int changeSocketCunt = select(0, &CopyReads, 0, 0, &Timeout);

		if (changeSocketCunt <= 0)
		{
			// 바뀐게 없으니
			//printf("Not Change\n");
			continue;
		}
		else
		{
			for (int i = 0; i < (int)Reads.fd_count; ++i)
			{
				if (FD_ISSET(Reads.fd_array[i], &CopyReads))
				{
					if (Reads.fd_array[i] == ListenSocket)
					{
						SOCKADDR_IN ClientSockAddr;
						ZeroMemory(&ClientSockAddr, sizeof(ClientSockAddr));
						int ClientSockAddrLength = sizeof(ClientSockAddr);

						SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);

						if (ClientSocket == INVALID_SOCKET)
						{
							cout << "Accept Error Number : " << GetLastError() << endl;
							continue;
						}

						FD_SET(ClientSocket, &Reads);
						CopyReads = Reads;

						char IP[1024] = { 0, };
						inet_ntop(AF_INET, &ClientSockAddr.sin_addr.s_addr, IP, 1024);
						cout << (unsigned short)ClientSocket << " connected : " << IP << endl;

						_beginthreadex(nullptr, 0, Thread_Server, (void*)&ClientSocket, 0, nullptr);

						//HANDLE ThreadHandles[2];
						//ThreadHandles[0] = (HANDLE)_beginthreadex(nullptr, 0, RecvThread, (void*)&ClientSocket, 0, nullptr);
						//ThreadHandles[1] = (HANDLE)_beginthreadex(nullptr, 0, SendThread, (void*)&ClientSocket, 0, nullptr);

						break;
					}
				}
			}
		}
	}

	closesocket(ListenSocket);
	WSACleanup();

	delete SQL_Connection;

	system("pause");

	return 0;
}


unsigned __stdcall Thread_Server(void* arg)
{
	cout << "---------- Thread Start ----------\n";

	SOCKET client = *(SOCKET*)arg;

	if (client == INVALID_SOCKET)
	{
		cout << "ClientSocket Error" << GetLastError() << endl;
	}

	unsigned short clientNum = (unsigned short)client;

	//SendMessage(client, "Connect Success");

	bool bSendSuccess = PacketMaker::SendPacket(&client, EPacket::ProgramStart, "Connect Success");
	if (bSendSuccess)
	{
		while (true)
		{
			unsigned short packetSize = 2;
			int recvByte = recv(client, (char*)(&packetSize), 2, MSG_WAITALL);
			if (recvByte <= 0)
			{
				RecvError(client);
				break;
			}

			packetSize = ntohs(packetSize);

			char* buffer = new char[packetSize];

			recvByte = recv(client, buffer, packetSize, MSG_WAITALL);
			if (recvByte <= 0)
			{
				RecvError(client);
			}

			unsigned short code = 0;
			memcpy(&code, buffer, 2);
			code = ntohs(code);

			// Data
			unsigned short dataSize = packetSize - 2;
			//bool bSendSuccess = false;

			switch ((EPacket)code)
			{
			case EPacket::None:
				break;
			case EPacket::C2S_IsLogin:
				cout << "test\n";
				PacketMaker::SendPacket(&client, EPacket::S2C_IsLogin);
				break;
			case EPacket::C2S_IsJoin:
				PacketMaker::SendPacket(&client, EPacket::S2C_IsJoin);
				break;
			case EPacket::C2S_ReqCheckID:
				// 데이터 베이스에서 아이디 검색
				if (true)
				{
					PacketMaker::SendPacket(&client, EPacket::S2C_ID_Success);
				}
				else
				{
					cout << "Client ID is incorrect. ";
					PacketMaker::SendPacket(&client, EPacket::S2C_ID_Failed);
				}
				break;
			case EPacket::C2S_ReqCheckPW:
				// 데이터 베이스에서 비밀번호 검색
				if (true)
				{
					PacketMaker::SendPacket(&client, EPacket::S2C_PW_Success);
				}
				else
				{
					cout << "Client PW is incorrect. ";
					PacketMaker::SendPacket(&client, EPacket::S2C_PW_Failed);
				}
				break;
			case EPacket::C2S_Make_NewID:
				// 데이터 베이스에서 아이디 검색
				if (true)
				{
					PacketMaker::SendPacket(&client, EPacket::S2C_Make_ID_Success);
				}
				else
				{
					cout << "Client ID is incorrect. ";
					PacketMaker::SendPacket(&client, EPacket::S2C_Make_ID_Failed);
				}
				break;
			case EPacket::C2S_Make_NewPW:
				// 데이터 베이스에서 비밀번호 검색
				if (true)
				{
					PacketMaker::SendPacket(&client, EPacket::S2C_Make_PW_Success);
				}
				else
				{
					cout << "Client PW is incorrect. ";
					PacketMaker::SendPacket(&client, EPacket::S2C_Make_PW_Failed);
				}
				break;
			case EPacket::C2S_Join_Success:
				PacketMaker::SendPacket(&client, EPacket::S2C_IsLogin);
				break;
			case EPacket::C2S_IsCanChat:
				// 대화할 상대가 있는지 확인한다.
				if (true)
				{
					PacketMaker::SendPacket(&client, EPacket::S2C_CanChat);
				}
				else
				{

				}
				break;
			case EPacket::C2S_Chat:
			{
				char message[PACKET_SIZE_MAX] = { 0, };
				memcpy(&message, buffer + 2, dataSize);

				cout << clientNum << "Send Chat : " << message << endl;

				string chatUserName = LoginUserList[clientNum].UserName;
				string chatMessage = chatUserName + " : " + message;
				PacketMaker::SendPacketAllClient(LoginUserList, EPacket::S2C_Chat, chatMessage.data(), clientNum);

				// Add to Log Data
				//string sqlQuery = "INSERT INTO chatlog(SenderName,Text) VALUES(?,?)";
				//sql::PreparedStatement* sql_PreState = SQL_Connection->prepareStatement(sqlQuery);
				//sql_PreState->setString(1, chatUserName);
				//sql_PreState->setString(2, message);
				//sql_PreState->execute();

				//delete sql_PreState;
			}
				break;
			case EPacket::Max:
				break;
			default:
				break;
			}
		}
	}
	else
	{
		SendError(client);
	}

	return 0;
}

unsigned __stdcall RecvThread(void* arg)
{
	SOCKET clientSocket = *(SOCKET*)arg;

	while (true)
	{
		char buffer[PACKET_SIZE_MAX] = { 0, };

		int recvByte = recv(clientSocket, buffer, PACKET_SIZE_MAX, MSG_WAITALL);

		if (recvByte <= 0)
		{
			cout << "Recv Error Num : " << GetLastError() << endl;
			closesocket(clientSocket);
			break;
		}

		cout << "Recv to Client : " << buffer << endl;
	}

	return 0;
}

unsigned __stdcall SendThread(void* arg)
{
	SOCKET clientSocket = *(SOCKET*)arg;

	while (true)
	{
		char message[PACKET_SIZE_MAX] = { "ServerSend\n" };

		//cin >> message;
		//cin.ignore();

		int sendByte = send(clientSocket, message, (int)strlen(message), 0);
		if (sendByte <= 0)
		{
			cout << "Send Error Num : " << GetLastError() << endl;
			closesocket(clientSocket);
			break;
		}

		cout << "Send to Client : " << message << endl;
	}

	return 0;
}

void SendMessage(SOCKET& socket, const char* str)
{
	int SendByte = send(socket, str, (int)(sizeof(str)), 0);

	if (SendByte <= 0)
	{
		SendError(socket);
	}
}

void RecvMessage(SOCKET& socket, char* str)
{
	int RecvByte = recv(socket, str, (int)sizeof(str), MSG_WAITALL);

	if (RecvByte <= 0)
	{
		RecvError(socket);
	}
	else
	{
		cout << "Recv to Client : " << recv << endl;
	}
}

void SendError(SOCKET& socket)
{
	cout << "Send Error : " << GetLastError() << endl;

	SOCKADDR_IN clientSockAddr;
	int clientSockAddrLength = sizeof(clientSockAddr);
	getpeername(socket, (SOCKADDR*)&clientSockAddr, &clientSockAddrLength);

	SOCKET errorSocket = socket;
	closesocket(socket);
	FD_CLR(socket, &Reads);
	CopyReads = Reads;

	char IP[1024] = { 0, };
	inet_ntop(AF_INET, &clientSockAddr.sin_addr.s_addr, IP, 1024);
	cout << "disconnected : " << IP << endl;
}

void RecvError(SOCKET& socket)
{
	cout << "Recv Error : " << GetLastError() << endl;

	SOCKADDR_IN clientSockAddr;
	int clientSockAddrLength = sizeof(clientSockAddr);
	getpeername(socket, (SOCKADDR*)&clientSockAddr, &clientSockAddrLength);

	SOCKET errorSocket = socket;
	closesocket(socket);
	FD_CLR(socket, &Reads);
	CopyReads = Reads;

	char IP[1024] = { 0, };
	inet_ntop(AF_INET, &clientSockAddr.sin_addr.s_addr, IP, 1024);
	cout << "disconnected : " << IP << endl;

	// 유저들의 오류 메세지 처리
	//if (TempUserList.count((unsigned short)DisconnectSocket) > 0)
	//{
	//	TempUserList.erase(TempUserList.find((unsigned short)DisconnectSocket));
	//}

	//string DisconnectUserNickName = UserList[(unsigned short)DisconnectSocket].NickName;

	//if (UserList.count((unsigned short)DisconnectSocket) > 0)
	//{
	//	UserList.erase(UserList.find((unsigned short)DisconnectSocket));
	//}

	//string broadCastMessage = DisconnectUserNickName + " has left.";
	//PacketMaker::SendPacketAllClient(UserList, EPacket::None, broadCastMessage.data());
}