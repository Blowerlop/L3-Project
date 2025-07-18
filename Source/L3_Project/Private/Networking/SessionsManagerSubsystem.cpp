// Fill out your copyright notice in the Description page of Project Settings.

#include "Networking/SessionsManagerSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "CharacterManagement/CharacterManagerSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "Networking/BaseGameInstance.h"

class UCharacterManagerSubsystem;
bool USessionsManagerSubsystem::IsSessionHost{};
bool USessionsManagerSubsystem::HasRunningSession{};
	
FName USessionsManagerSubsystem::RunningSessionName{};

void USessionsManagerSubsystem::CreateSession(FName SessionName, FCreateSessionDelegate Delegate, FName KeyName,
                                              FString KeyValue, bool bDedicatedServer)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	UE_LOG(LogTemp, Error, TEXT("Creating session subsystem name: %s"), *Subsystem->GetSubsystemName().ToString());
	
	BP_CreateSessionDelegate = Delegate;
    CreateSessionDelegateHandle =
        SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(
            this,&ThisClass::HandleCreateSessionCompleted));

    // Session settings 
    TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
    SessionSettings->NumPublicConnections = MaxNumberOfPlayersInSession;
	SessionSettings->bIsLANMatch = Subsystem->GetSubsystemName() == "NULL"; // If the session is LAN or not
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

void USessionsManagerSubsystem::HandleCreateSessionCompleted(FName EosSessionName, bool bWasSuccessful)
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

void USessionsManagerSubsystem::DestroySession()
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

		DestroySessionDelegate.ExecuteIfBound(false);
		DestroySessionDelegate.Unbind();
		
		BP_DestroySessionDelegate.ExecuteIfBound(false);
		BP_DestroySessionDelegate.Unbind();
		
		Session->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
		DestroySessionDelegateHandle.Reset();
	}
}

void USessionsManagerSubsystem::DestroySessionWithBPCallback(FDestroySessionDelegateBP Delegate)
{
	BP_DestroySessionDelegate = Delegate;
	DestroySession();
}

void USessionsManagerSubsystem::DestroySessionWithCallback(const DSWCFunc& Func)
{
	DestroySessionDelegate.BindLambda(Func);
	DestroySession();
}

void USessionsManagerSubsystem::HandleDestroySessionCompleted(FName EosSessionName, bool bWasSuccessful)
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

	BP_DestroySessionDelegate.ExecuteIfBound(bWasSuccessful);
	BP_DestroySessionDelegate.Unbind();

	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
}

void USessionsManagerSubsystem::FindSessions(FName SearchKey, FString SearchValue, FFindSessionsDelegate Delegate)
{
	BP_FindSessionsDelegate = Delegate;
	FindSessions(SearchKey, SearchValue);
}

void USessionsManagerSubsystem::FindSessions(FName SearchKey, FString SearchValue, const FindSessionFunc& Func)
{
	CPP_FindSessionsDelegate.BindLambda(Func);
	FindSessions(SearchKey, SearchValue);
}

void USessionsManagerSubsystem::FindSessions(FName SearchKey, FString SearchValue)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	TSharedRef<FOnlineSessionSearch> Search = MakeShared<FOnlineSessionSearch>();
 
	// Remove the default search parameters that FOnlineSessionSearch sets up.
	Search->QuerySettings.SearchParams.Empty();
	Search->bIsLanQuery = Subsystem->GetSubsystemName() == "NULL"; // If the session is LAN or not
 
	Search->QuerySettings.Set(SearchKey, SearchValue, EOnlineComparisonOp::Equals); // Key/Value used to get a specific session
	FindSessionsDelegateHandle =
		SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleFindSessionsCompleted,
			Search));
    
	UE_LOG(LogTemp, Log, TEXT("Finding sessions..."));
    
	if (!SessionInterface->FindSessions(0, Search))
	{
		UE_LOG(LogTemp, Error, TEXT("Find session failed"));
	}
}

void USessionsManagerSubsystem::HandleFindSessionsCompleted(bool bWasSuccessful, TSharedRef<FOnlineSessionSearch> Search)
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

		auto OneValidSession = false;
		
		for (const auto& SessionInSearchResult : Search->SearchResults)
		{
			if (!IsValidSession(Subsystem, SessionInSearchResult, Search))
			{
				UE_LOG(LogTemp, Warning, TEXT("session not validated params:"))
				
				for (auto [Name, Setting] : SessionInSearchResult.Session.SessionSettings.Settings)
				{
					UE_LOG(LogTemp, Warning, TEXT("   - %s %s"), *Name.ToString(), *Setting.Data.ToString())
				}
				
				continue;
			}
			UE_LOG(LogTemp, Log, TEXT("Found one session."));

			const auto BPResults = FBlueprintSessionSearchResult::GetFromCPP(SessionInSearchResult);
			
			(void)BP_FindSessionsDelegate.ExecuteIfBound(true, BPResults);
			(void)CPP_FindSessionsDelegate.ExecuteIfBound(true, BPResults);

			OneValidSession = true;
			break;
		}

		if (!OneValidSession)
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't find any valid session."));

			(void)BP_FindSessionsDelegate.ExecuteIfBound(false, {});
			(void)CPP_FindSessionsDelegate.ExecuteIfBound(false, {});
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Find Sessions failed."));
	}
 
	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsDelegateHandle);
	FindSessionsDelegateHandle.Reset();
}

void USessionsManagerSubsystem::JoinSession(FName SessionName, FBlueprintSessionSearchResult SessionData)
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

void USessionsManagerSubsystem::RegisterSelf()
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();

	if(APlayerController* PlayerController = GetWorld()->GetFirstPlayerController(); PlayerController != nullptr)
	{
		if(const auto ID = IdentityInterface->GetUniquePlayerId(0); ID.IsValid())
		{
			GetWorld()->GetAuthGameMode()->GameSession->RegisterPlayer(PlayerController, FUniqueNetIdRepl(ID), false);
		}
	}
}

void USessionsManagerSubsystem::HandleJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
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
			if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				const auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
				if (!IsValid(GameInstance))
				{
					UE_LOG(LogTemp, Error, TEXT("Can't client travel: GameInstance is not a UBaseGameInstance."));
					return;
				}

				const auto CharacterManager = GameInstance->GetSubsystem<UCharacterManagerSubsystem>();
				
				ConnectInfos.Append(FString::Printf(TEXT("?%s=%s"), *UBaseGameInstance::UUIDConnectOptionsKey,
					*GameInstance->SelfClientData.UUID));
				
				ConnectInfos.Append(FString::Printf(TEXT("?%s=%s"), *UBaseGameInstance::UserNameConnectOptionsKey,
					*GameInstance->SelfClientData.Name));
				
				ConnectInfos.Append(FString::Printf(TEXT("?%s=%s"), *UBaseGameInstance::CharacterUUIDConnectOptionsKey,
					*CharacterManager->SelectedCharacter->UUID));
				
				UE_LOG(LogTemp, Error, TEXT("Client travel to %s"), *ConnectInfos);
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

bool USessionsManagerSubsystem::IsValidSession(const IOnlineSubsystem* Subsystem, const FOnlineSessionSearchResult& Result,
	TSharedRef<FOnlineSessionSearch> Search)
{
	if (Subsystem->GetSubsystemName() != "NULL") return true;
	
	FString Value{};
	
	for (auto [Name, Params] : Search->QuerySettings.SearchParams)
	{
		if (!Result.Session.SessionSettings.Get(Name, Value) || Value != Params.Data.ToString())
		{
			return false;
		}
	}

	return true;
}

void USessionsManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	HasRunningSession = false;
	IsSessionHost = false;
	RunningSessionName = "";
}

void USessionsManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	
}
