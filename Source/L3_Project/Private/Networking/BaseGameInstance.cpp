// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/BaseGameInstance.h"

#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionDelegates.h"

#pragma region Static

bool UBaseGameInstance::IsSessionHost{};
bool UBaseGameInstance::HasRunningSession{};
	
int UBaseGameInstance::InstanceSessionID{};
	
FName UBaseGameInstance::RunningSessionName{};

#pragma endregion

#pragma region Login/out

void UBaseGameInstance::Login(const bool bUseDevTool, FString AuthToolId) const
{
	if (const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface(); IdentityInterface.IsValid())
		{
			FOnlineAccountCredentials Credentials;

			if (bUseDevTool)
			{
				Credentials.Type = TEXT("developer");
				Credentials.Id = "localhost:8080";
				Credentials.Token = AuthToolId;
			}
			else
			{
				Credentials.Type = TEXT("deviceID");
				Credentials.Id = "";
				Credentials.Token = "";
			}
				
			IdentityInterface->Login(0, Credentials);
		}
	}
}

void UBaseGameInstance::Logout() const
{
	if (const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface(); IdentityInterface.IsValid())
		{
			IdentityInterface->Logout(0);
		}
	}
}

bool UBaseGameInstance::IsLoggedIn() const
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();

	return IdentityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}

#pragma endregion 

#pragma region Session Management

void UBaseGameInstance::CreateSession(const FName SessionName, const FCreateSessionDelegate Delegate, const FName KeyName,
                                      FString KeyValue, const bool bDedicatedServer)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	BP_CreateSessionDelegate = Delegate;
    CreateSessionDelegateHandle =
        SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(
            this,&ThisClass::HandleCreateSessionCompleted));

    // Session settings 
    TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
    SessionSettings->NumPublicConnections = MaxNumberOfPlayersInSession;
    SessionSettings->bShouldAdvertise = true; //True = searchable, False = unsearchable
    SessionSettings->bUsesPresence = false;   //Idk
    SessionSettings->bAllowJoinViaPresence = false; // superset by bShouldAdvertise and will be true on the backend
    SessionSettings->bAllowJoinViaPresenceFriendsOnly = false; // superset by bShouldAdvertise and will be true on the backend
    SessionSettings->bAllowInvites = false;    // Don't need it
    SessionSettings->bAllowJoinInProgress = false; // Allow or not join after the session has started
    SessionSettings->bIsDedicated = bDedicatedServer; // Dedicated server or not
    SessionSettings->bUseLobbiesIfAvailable = false; // Nop only sessions. Lobbies not available on dedicated servers.
    SessionSettings->bUseLobbiesVoiceChatIfAvailable = false; // Don't care
    SessionSettings->bUsesStats = true; // Idk but ok

	// What clients will search to find the session
    SessionSettings->Settings.Add(KeyName, FOnlineSessionSetting((KeyValue), EOnlineDataAdvertisementType::ViaOnlineService));

	UE_LOG(LogTemp, Log, TEXT("Creating session... %s %s"), *SessionName.ToString(), *KeyValue);

    if (!SessionInterface->CreateSession(0, SessionName, *SessionSettings))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create session!"));
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
        CreateSessionDelegateHandle.Reset();
    }
}

void UBaseGameInstance::HandleCreateSessionCompleted(const FName EosSessionName, const bool bWasSuccessful)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	
	if (bWasSuccessful)
	{
		IsSessionHost = true;
		
		HasRunningSession = true;
		RunningSessionName = EosSessionName;
		
		UE_LOG(LogTemp, Log, TEXT("Session: %s Created!"), *EosSessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create session!"));
	}

	 (void)BP_CreateSessionDelegate.ExecuteIfBound(EosSessionName, bWasSuccessful);
	
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
	CreateSessionDelegateHandle.Reset();
}

void UBaseGameInstance::DestroySession()
{
	UE_LOG(LogTemp, Log, TEXT("Destroying session..."));
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	DestroySessionDelegateHandle =
		Session->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleDestroySessionCompleted));

	if (!Session->DestroySession(RunningSessionName))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy session.")); 
		Session->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
		DestroySessionDelegateHandle.Reset();
	}
}

void UBaseGameInstance::DestroySessionWithCallback(const FDestroySessionDelegate& Delegate)
{
	DestroySessionDelegate = Delegate;
	DestroySession();
}

void UBaseGameInstance::HandleDestroySessionCompleted(const FName EosSessionName, const bool bWasSuccessful)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (bWasSuccessful)
	{
		IsSessionHost = false;
		HasRunningSession = false;
		UE_LOG(LogTemp, Log, TEXT("Destroyed session succesfully. %s"), *EosSessionName.ToString()); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy session.")); 
	}

	(void)DestroySessionDelegate.ExecuteIfBound(bWasSuccessful);
	DestroySessionDelegate.Unbind();

	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
}

#pragma endregion

#pragma region Querying Sessions

void UBaseGameInstance::FindSessions(const FName SearchKey, const FString SearchValue, const FFindSessionsDelegate Delegate)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	TSharedRef<FOnlineSessionSearch> Search = MakeShared<FOnlineSessionSearch>();
 
	// Remove the default search parameters that FOnlineSessionSearch sets up.
	Search->QuerySettings.SearchParams.Empty();
 
	Search->QuerySettings.Set(SearchKey, SearchValue, EOnlineComparisonOp::Equals); // Key/Value used to get a specific session
	FindSessionsDelegateHandle =
		SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleFindSessionsCompleted,
			Search));
    
	UE_LOG(LogTemp, Log, TEXT("Finding sessions..."));

	BP_FindSessionsDelegate = Delegate;
    
	if (!SessionInterface->FindSessions(0, Search))
	{
		UE_LOG(LogTemp, Error, TEXT("Find session failed"));
	}
}

void UBaseGameInstance::HandleFindSessionsCompleted(const bool bWasSuccessful, const TSharedRef<FOnlineSessionSearch> Search)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
 
	if (bWasSuccessful)
	{
		const auto Num = Search->SearchResults.Num();

		UE_LOG(LogTemp, Log, TEXT("Find sessions successful. Found %d sessions."), Num);

		if (Num == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found 0 sessions."));
			(void)BP_FindSessionsDelegate.ExecuteIfBound(false, {});
			(void)CPP_FindSessionsDelegate.ExecuteIfBound(false, {});
		}
		
		for (const auto SessionInSearchResult : Search->SearchResults)
		{
			// Just get the first session found. There should not be more than one session corresponding to given search key/value.
			UE_LOG(LogTemp, Log, TEXT("Found one session."));

			const auto BPResults = FBlueprintSessionSearchResult::GetFromCPP(SessionInSearchResult);
			
			(void)BP_FindSessionsDelegate.ExecuteIfBound(true, BPResults);
			(void)CPP_FindSessionsDelegate.ExecuteIfBound(true, BPResults);
			break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Find Sessions failed."));
	}
 
	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsDelegateHandle);
	FindSessionsDelegateHandle.Reset();
}

void UBaseGameInstance::JoinSession(const FName SessionName, const FBlueprintSessionSearchResult SessionData)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
 
	JoinSessionDelegateHandle = 
		SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleJoinSessionCompleted));
    
	UE_LOG(LogTemp, Log, TEXT("Joining session... %s"), *SessionName.ToString());
	
	if (!SessionInterface->JoinSession(0, SessionName, SessionData.OnlineResult))
	{
		UE_LOG(LogTemp, Error, TEXT("Join session failed"));
	} 
}

void UBaseGameInstance::HandleJoinSessionCompleted(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Log, TEXT("Joined session. %s"), *SessionName.ToString());
		
		HasRunningSession = true;
		RunningSessionName = SessionName;

		FString ConnectInfos{};
		SessionInterface->GetResolvedConnectString(SessionName, ConnectInfos);
		
		if (!ConnectInfos.IsEmpty())
		{
			if (APlayerController* PlayerController = GetFirstLocalPlayerController())
			{
				UE_LOG(LogTemp, Log, TEXT("Client travel to %s"), *ConnectInfos);
				PlayerController->ClientTravel(ConnectInfos, TRAVEL_Absolute);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Can't client travel: can't find player controller."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Resolved connect string is empty for session. %s"), *SessionName.ToString());
		}
	}
	
	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionDelegateHandle);
	JoinSessionDelegateHandle.Reset();
}

#pragma endregion

#pragma region Lifecycle

void UBaseGameInstance::Shutdown()
{
	Super::Shutdown();
	
	if (HasRunningSession)
	{
		DestroySession();
	}

	if (!IsRunningDedicatedServer())
	{
		Logout();
	}
}

#pragma endregion