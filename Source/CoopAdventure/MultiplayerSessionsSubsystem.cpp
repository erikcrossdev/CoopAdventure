// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"


UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	CreateServerAfterDestroy = false;
	DestroyServerName = "";
	ServerNameToFind = "";
	MySessionName = FName("Co-op Adventure Session Name");
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
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, 
				&UMultiplayerSessionsSubsystem::OnCreateSessionComplete); //bind to the delegate
			
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, 
				&UMultiplayerSessionsSubsystem::OnDestroySessionComplete); //bind to the delegate
		
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,
				&UMultiplayerSessionsSubsystem::OnFindSessionsComplete);

			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,
				&UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
		}
	}
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
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

	FNamedOnlineSession *ExistingSession = SessionInterface->GetNamedSession(MySessionName);

	if (ExistingSession) {
		FString Msg = FString::Printf(TEXT("Session alread exist, destroy it: %s"), *MySessionName.ToString());
		PrintString(Msg);
		CreateServerAfterDestroy = true;
		DestroyServerName = ServerName;
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
	PrintString(FString::Printf(TEXT("IsLAN? %d"), SessionSettings.bIsLANMatch));
	//Add a session name as a key-value-pair
	SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName) {
	PrintString("FindServer");

	if (ServerName.IsEmpty()) {
		PrintString("Server name cannot be empty");
		return;
	}
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool isLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName().ToString().ToUpper() == "NULL") {
		isLAN = true;
	}
	SessionSearch->bIsLanQuery = isLAN;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	ServerNameToFind = ServerName;

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), WasSuccessful));
		
	UE_LOG(LogTemp, Warning, TEXT("IsServer: %s"), GetWorld()->IsServer() ? TEXT("true") : TEXT("false"));
UE_LOG(LogTemp, Warning, TEXT("NetMode: %d"), (int)GetWorld()->GetNetMode());
GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	if (WasSuccessful) 
	{
		//UE_LOG(LogTemp, Warning, TEXT("HasAuthority: %s"), HasAuthority() ? TEXT("true") : TEXT("false"));
		//GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");//Load level
		if(GetWorld()->IsServer()) {
			GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("WARNING: Tried ServerTravel on CLIENT!"));
		}
	}

}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool WasSuccessful)
{
	PrintString(FString::Printf(TEXT("On Destroy Session Complete: %d, Success: %d"), *SessionName.ToString(), WasSuccessful));

	if (CreateServerAfterDestroy) {
		CreateServerAfterDestroy = false;
		CreateServer(DestroyServerName);
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool WasSuccessful)
{
	if (!WasSuccessful) return;
	if (ServerNameToFind.IsEmpty()) return;
	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
	FOnlineSessionSearchResult* CorrectResult = 0;

	if (Results.Num() > 0) {
		FString Msg = FString::Printf(TEXT("%d sessions found"), Results.Num());
		PrintString(Msg);

		for (FOnlineSessionSearchResult Result : Results) 
		{
			if (Result.IsValid()) 
			{
				FString ServerName = "no-name";
				Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);						

				if (ServerName.Equals(ServerNameToFind)) {
					CorrectResult = &Result;
					FString Msg2 = FString::Printf(TEXT("Found server with name: %s"), *ServerName);
					PrintString(Msg2);
					break;
				}
			}
		}

		if (CorrectResult) {
			SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
		}
		else {
			PrintString(FString::Printf(TEXT("Could not find server named %s"), *ServerNameToFind));
			ServerNameToFind = "";
		}
	}
	else {
		PrintString("No session found");
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success) {
		PrintString(FString::Printf(TEXT("Successfully Joined session %s"), *SessionName.ToString()));	
		
		//connect to other player
		FString Address = ""; //an ip address
		bool Success = SessionInterface->GetResolvedConnectString(MySessionName, Address);
		if (Success) {
			PrintString(FString::Printf(TEXT("Address is %s"), *Address));
			APlayerController * PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController) {

				PrintString("Client is traveling");
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);//travel the player
			}
		}
		else {
			PrintString("GetResolvedConnectedString failed");
		}
	
	}
	else 
	{
		PrintString("Failed to join session :/");
	}
}

