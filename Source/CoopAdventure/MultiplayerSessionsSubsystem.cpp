// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	//PrintString("MMS Constructor");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//PrintString("MMS Initialize");
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get(); //return the online subsystem
	if (OnlineSubsystem) 
	{
		FString SubSystemName = OnlineSubsystem->GetSubsystemName().ToString(); //the value set in the ini file, steam in our case
		PrintString(SubSystemName);

		SessionInterface = OnlineSubsystem->GetSessionInterface(); //pointer to the session interface
		if (SessionInterface.IsValid()) {
			PrintString("Session Interface is valid");
		}
	}
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	//UE_LOG(LogTemp, Warning, TEXT("MMS Deinitialize"));
}

void UMultiplayerSessionsSubsystem::PrintString(const FString& message)
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, message);
	}
}

void UMultiplayerSessionsSubsystem::CreateServer(FString ServerName) {
	PrintString("CreateServer");
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName) {
	PrintString("FindServer");
}