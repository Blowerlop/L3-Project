// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/BaseGameSession.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Networking/BaseGameInstance.h"
#include "Networking/SessionsManagerSubsystem.h"


void ABaseGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId,
                                      const bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
    
	if (!USessionsManagerSubsystem::IsSessionHost)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Registering new player..."));
	
	if (!UniqueId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Can't register player: UniqueId is not valid!"));
		return;
	}
	
	const IOnlineSubsystem* Subsystem = GetOnlineSubsystem();
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        
	RegisterPlayerDelegateHandle =
		SessionInterface->AddOnRegisterPlayersCompleteDelegate_Handle(FOnRegisterPlayersCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleRegisterPlayerCompleted));
        
	if (!SessionInterface->RegisterPlayer(USessionsManagerSubsystem::RunningSessionName, *UniqueId, false))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Register Player!"));
		SessionInterface->ClearOnRegisterPlayersCompleteDelegate_Handle(RegisterPlayerDelegateHandle);
		RegisterPlayerDelegateHandle.Reset();
	}
}

void ABaseGameSession::HandleRegisterPlayerCompleted(FName EosSessionName, const TArray<FUniqueNetIdRef>& PlayerIds,
                                                     const bool bWasSuccessful)
{
	const IOnlineSubsystem* Subsystem = GetOnlineSubsystem();
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
 
	if (bWasSuccessful)
	{
		RegisteredPlayerCount++;
		UE_LOG(LogTemp, Log, TEXT("Player registered in EOS Session!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to register player! After callback."));
	}
    
	SessionInterface->ClearOnRegisterPlayersCompleteDelegate_Handle(RegisterPlayerDelegateHandle);
	RegisterPlayerDelegateHandle.Reset();
}

void ABaseGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
	Super::UnregisterPlayer(ExitingPlayer);
	
	if (!USessionsManagerSubsystem::IsSessionHost || !USessionsManagerSubsystem::HasRunningSession)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Unregistering player..."));
	
	const IOnlineSubsystem* Subsystem = GetOnlineSubsystem();
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	if (!Session.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Can't unregister player! Session interface is not valid!"));
		return;
	}
		
	if (!ExitingPlayer->PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't unregister player! ExitingPlayer PlayerState is not valid!"));
		return;
	}

	const auto ID = ExitingPlayer->PlayerState->GetUniqueId();
	if (!ID.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Can't unregister player! ExitingPlayer->PlayerState->GetUniqueId() is not valid!"));
		return;
	}
		
	UnregisterPlayerDelegateHandle =
		Session->AddOnUnregisterPlayersCompleteDelegate_Handle(FOnUnregisterPlayersCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleUnregisterPlayerCompleted));
		
	if (!Session->UnregisterPlayer(USessionsManagerSubsystem::RunningSessionName, *ID))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Unregister Player!"));
		Session->ClearOnUnregisterPlayersCompleteDelegate_Handle(UnregisterPlayerDelegateHandle);
		UnregisterPlayerDelegateHandle.Reset();
	}
}

void ABaseGameSession::HandleUnregisterPlayerCompleted(FName EosSessionName, const TArray<FUniqueNetIdRef>& PlayerIds,
                                                       const bool bWasSuccessful)
{
	const IOnlineSubsystem* Subsystem = GetOnlineSubsystem();
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (bWasSuccessful)
	{
		RegisteredPlayerCount--;
        
		UE_LOG(LogTemp, Log, TEXT("Player unregistered in EOS Session!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to unregister player! After callback."));
	}
    
	SessionInterface->ClearOnUnregisterPlayersCompleteDelegate_Handle(UnregisterPlayerDelegateHandle);
	UnregisterPlayerDelegateHandle.Reset();
}

void ABaseGameSession::NotifyLogout(const APlayerController* PC)
{
	Super::NotifyLogout(PC);
}

void ABaseGameSession::StartSession()
{
	const IOnlineSubsystem* Subsystem = GetOnlineSubsystem();
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
 
	StartSessionDelegateHandle =
		SessionInterface->AddOnStartSessionCompleteDelegate_Handle(FOnStartSessionCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleStartSessionCompleted)); 
    
	if (!SessionInterface->StartSession(SessionName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to start session!"));
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionDelegateHandle);
		StartSessionDelegateHandle.Reset();		
	}
}

void ABaseGameSession::HandleStartSessionCompleted(FName EosSessionName, const bool bWasSuccessful)
{
	const IOnlineSubsystem* Subsystem = GetOnlineSubsystem();
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session Started!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to start session! After callback."));
	}
 
	SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionDelegateHandle);
	StartSessionDelegateHandle.Reset();
}

void ABaseGameSession::EndSession()
{
	const IOnlineSubsystem* Subsystem = GetOnlineSubsystem();
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	EndSessionDelegateHandle =
		SessionInterface->AddOnEndSessionCompleteDelegate_Handle(FOnEndSessionCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleEndSessionCompleted));

	if (!SessionInterface->EndSession(SessionName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to end session! After callback."));
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(StartSessionDelegateHandle);
		EndSessionDelegateHandle.Reset();
	}
}

void ABaseGameSession::HandleEndSessionCompleted(FName EosSessionName, const bool bWasSuccessful)
{
	const IOnlineSubsystem* Subsystem = GetOnlineSubsystem();
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session ended!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to end session! After callback."));
	}

	SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(EndSessionDelegateHandle);
	EndSessionDelegateHandle.Reset();
}

// Can't make this pure virtual. Not allowed in UCLASS
FName ABaseGameSession::GetOnlineSubsystemName() const
{
	return "";
}