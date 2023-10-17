// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TCPCommunicatonManager.generated.h"

UCLASS()
class LINKTCPSERVER_API ATCPCommunicatonManager : public AActor
{
	GENERATED_BODY()
private:
	class FSocket* Socket;

	FString ServerIP;
	int32 ServerPort;
	
public:	
	// Sets default values for this actor's properties
	ATCPCommunicatonManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool ConnecttoServer();
	bool ProcessReceiveData(FString& Data);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Send to TCPServer
	UFUNCTION(BlueprintCallable)
	void SendtoServer(const FString& Data);

	// Receive from TCPServer
	bool RecvfromServer(FString& Data);
};
