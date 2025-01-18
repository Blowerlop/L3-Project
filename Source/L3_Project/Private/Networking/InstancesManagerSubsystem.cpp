// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/InstancesManagerSubsystem.h"

#include "OnlineSubsystemUtils.h"
#include "GroupManagement/InstancesUserComponent.h"
#include "Instances/InstanceDataAsset.h"
#include "Networking/BaseGameInstance.h"
#include "Networking/SessionsManagerSubsystem.h"

int UInstancesManagerSubsystem::InstanceIDCounter{};
int UInstancesManagerSubsystem::InstanceSessionID{};
bool UInstancesManagerSubsystem::IsInstanceBeingDestroyed{};

void UInstancesManagerSubsystem::StartNewInstance(int SessionID, UInstanceDataAsset* Data)
{
	UBaseGameInstance* GameInstance;
	if (!TryGetBaseGameInstance(GameInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to get game instance"));
		return;
	}

	const auto SessionManager = GameInstance->GetSubsystem<USessionsManagerSubsystem>();
	if (!SessionManager) return;
	
	if (!GameInstance->IsLoggedIn())
	{
		UE_LOG(LogTemp, Error, TEXT("Not logged in"));
		return;
	}

	auto OnTransition = [this, SessionID, Data]() {
		UE_LOG(LogTemp, Log, TEXT("Transition completed. Starting new instance."));
		StartListenServer(SessionID, Data->MapPath);
	};
	
	auto OnSessionDestroyed = [this, GameInstance, OnTransition](const bool bWasSuccessful) {
		if(!bWasSuccessful)
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to destroy session. Going back to main menu."));

			// Go back to main menu
			return;
		}
		
		GameInstance->StartTransition(ENetTransitionType::LobbyToInstance, OnTransition);
	};
	
	SessionManager->DestroySessionWithCallback(OnSessionDestroyed, LobbyOnlineSubsystem);
}

void UInstancesManagerSubsystem::StartListenServer(const int SessionID, const FString& InstanceMapPath) const
{
	if (USessionsManagerSubsystem::HasRunningSession)
	{
		UE_LOG(LogTemp, Error, TEXT("Already in a session"));
		return;
	}

	if (UWorld* World = GetWorld(); World != nullptr)
	{
		InstanceSessionID = SessionID;
		
		const FURL ListenURL(nullptr, *(InstanceMapPath + "?listen"), TRAVEL_Absolute);
		World->ServerTravel(ListenURL.ToString());
	}
}

void UInstancesManagerSubsystem::SetHostingType(const FString& Args)
{
	// Those are commented because UWorld::HostingType doesn't exist in regular Unreal Engine code base.
	// It was added for the project only chez moi
	
	if (Args == "EOS")
	{
		//UWorld::HostingType = EHostingType::EOS;
	}
	else if (Args == "LAN")
	{
		//UWorld::HostingType = EHostingType::LANBroadcast;
	}
	else if (Args == "IP")
	{
		//UWorld::HostingType = EHostingType::IP;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid hosting type"));
	}
}

void UInstancesManagerSubsystem::StopInstance()
{
	USessionsManagerSubsystem* SessionManager;
	if (!TryGetSessionsManager(SessionManager))
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to get session manager"));
		return;
	}
	
	if (!SessionManager->HasRunningSession)
	{
		UE_LOG(LogTemp, Error, TEXT("No running session"));
		return;
	}

	if (!SessionManager->IsSessionHost)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't stop instance. Not a host"));
		return;
	}

	if (GetWorld()->GetNumPlayerControllers() == 1)
	{
		const auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
		if (!GameInstance) return;
		
		const auto InstancesManager = GameInstance->GetSubsystem<UInstancesManagerSubsystem>();
		if (!InstancesManager) return;
		
		InstancesManager->ReturnToLobby();
		return;
	}

	IsInstanceBeingDestroyed = true;

	for(FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* Controller = Iterator->Get();

		// "IsLocalPlayerController" = do not send to host. Host will disconnect automatically after everyone has left
		if (Controller && !Controller->IsLocalPlayerController() && Controller->IsPrimaryPlayer())
		{
			const auto InstancesUser = Controller->GetPawn()->GetComponentByClass<UInstancesUserComponent>();

			if (!IsValid(InstancesUser))
			{
				UE_LOG(LogTemp, Error, TEXT("No InstancesUserComponent found on player controller pawn! Can't disconnect it ??????"));
				continue;
			}
			
			InstancesUser->ReturnToLobbyClientRPC();
		}
	}
}

void UInstancesManagerSubsystem::JoinInstance(FName SessionName, FBlueprintSessionSearchResult SessionData)
{
	UBaseGameInstance* GameInstance;
	if (!TryGetBaseGameInstance(GameInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to get game instance"));
		return;
	}

	auto SessionManager = GameInstance->GetSubsystem<USessionsManagerSubsystem>();
	if (!SessionManager) return;
	
	if (!GameInstance->IsLoggedIn())
	{
		UE_LOG(LogTemp, Error, TEXT("Not logged in"));
		return;
	}

	auto OnTransition = [this, SessionName, SessionData, SessionManager]() {
		SessionManager->JoinSession(SessionName, SessionData, GetOnlineSubsystemName());
	};
	
	auto OnSessionDestroyed = [this, OnTransition, GameInstance](const bool bWasSuccessful) {
		if (!bWasSuccessful)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to destroy session. Return to main menu."));

			// Go back to main menu
			return;
		}
		
		GameInstance->StartTransition(ENetTransitionType::LobbyToInstance, OnTransition);
	};
	
	SessionManager->DestroySessionWithCallback(OnSessionDestroyed, LobbyOnlineSubsystem);
}

void UInstancesManagerSubsystem::ReturnToLobby()
{
	UBaseGameInstance* GameInstance;
	if (!TryGetBaseGameInstance(GameInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to get game instance"));
		return;
	}

	auto SessionManager = GameInstance->GetSubsystem<USessionsManagerSubsystem>();
	if (!SessionManager) return;
	
	if (!GameInstance->IsLoggedIn())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not logged in"));
		return;
	}

	IsInstanceBeingDestroyed = false;
	
	auto OnSessionFound = [this, SessionManager](const bool bWasSuccessful, const FBlueprintSessionSearchResult& Search)
	{
		if (!bWasSuccessful)
		{
			UE_LOG(LogTemp, Log, TEXT("No session found."));

			// Go back to main menu
			return;
		}

		SessionManager->JoinSession("Lobby", Search, LobbyOnlineSubsystem);
	};
	
	auto OnTransition = [this, OnSessionFound, SessionManager]() {
		SessionManager->FindSessions("TYPE", "Lobby", OnSessionFound, LobbyOnlineSubsystem);
	};
	
	auto OnSessionDestroyed = [this, OnTransition, GameInstance](const bool bWasSuccessful) {
		if (!bWasSuccessful)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to destroy session. Return to main menu."));

			// Go back to main menu
			return;
		}
		
		GameInstance->StartTransition(ENetTransitionType::LobbyToInstance, OnTransition);
	};
	
	SessionManager->DestroySessionWithCallback(OnSessionDestroyed, GetOnlineSubsystemName());
}


