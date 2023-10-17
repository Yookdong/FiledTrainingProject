// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerManager/TCPCommunicatonManager.h"
#include "Networking.h"
#include "Sockets.h"

// Sets default values
ATCPCommunicatonManager::ATCPCommunicatonManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ServerIP = TEXT("127.0.0.1");
	ServerPort = 7777;
}

// Called when the game starts or when spawned
void ATCPCommunicatonManager::BeginPlay()
{
	Super::BeginPlay();

	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCPClientSocket"), false);
	
	if (!Socket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket Error"));
	}

	if (!ConnecttoServer())
	{
		UE_LOG(LogTemp, Warning, TEXT("Connect Error"));
	}
}

bool ATCPCommunicatonManager::ConnecttoServer()
{
	TSharedRef<FInternetAddr> Address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValidIP = false;
	Address->SetIp(*ServerIP, bIsValidIP);
	Address->SetPort(ServerPort);

	if (bIsValidIP)
	{
		if (Socket->Connect(*Address))
		{
			// Connection successful
			return true;
		}
		else
		{
			// Handle connection error
		}
	}
	else
	{
		// Handle invalid IP address error
	}
	return false;
}

bool ATCPCommunicatonManager::ProcessReceiveData(FString& Data)
{
	// Implement data processing logic here
	return true; // Return true if processing was successful
}

// Called every frame
void ATCPCommunicatonManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString ReceivedData;
	if (RecvfromServer(ReceivedData))
	{
		ProcessReceiveData(ReceivedData);
	}
}

void ATCPCommunicatonManager::SendtoServer(const FString& Data)
{
	if (Socket)
	{
		int32 BytesSent = 0;
		if (!Socket->Send(reinterpret_cast<const uint8*>(Data.GetCharArray().GetData()), Data.Len(), BytesSent))
		{
			// Handle sending data error
		}
	}
}

bool ATCPCommunicatonManager::RecvfromServer(FString& Data)
{
	if (Socket)
	{
		uint32 Size;
		if (Socket->HasPendingData(Size))
		{
			uint8* Buffer = new uint8[Size];
			int32 BytesRead = 0;
			if (Socket->Recv(Buffer, Size, BytesRead))
			{
				Data = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(Buffer)));
				delete[] Buffer;
				return true;
			}
			else
			{
				// Handle receiving data error
			}
		}
	}
	return false;
}

