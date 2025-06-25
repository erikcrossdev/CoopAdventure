// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	//PrintString("MMS Constructor");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//PrintString("MMS Initialize");
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(); //return the online subsystem
	if (OnlineSubsystem)
	{
		FString SubSystemName = OnlineSubsystem->GetSubsystemName().ToString(); //the value set in the ini file, steam in our case
		PrintString(SubSystemName);

		SessionInterface = OnlineSubsystem->GetSessionInterface(); //pointer to the session interface
		if (SessionInterface.IsValid()) {
			PrintString("Session Interface is valid");
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete); //bind to the delegate
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

	if (ServerName.IsEmpty()) 
	{
		PrintString("ServerName cannot be Empty");
		return;
	}
	FName MySessionName = FName("Co-op Adventure Session Name");

	FNamedOnlineSession *ExistingSession = SessionInterface->GetNamedSession(MySessionName);

	if (ExistingSession) {
		FString Msg = FString::Printf(TEXT("Session alread exist, destroy it: %s"), *MySessionName.ToString());
		PrintString(Msg);
		SessionInterface->DestroySession(MySessionName); //but destroy session is a delegate and async
		//we need to se if this async function was completed before creating the session
		return;
	}
	

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true; //others can see the match?
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	FString SubSystemName = OnlineSubsystem->GetSubsystemName().ToString().ToLower();

	SessionSettings.bIsLANMatch = SubSystemName == "steam"? false : true;

	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName) {
	PrintString("FindServer");
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), WasSuccessful));

	//Load level
	if (WasSuccessful) 
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	}
}
