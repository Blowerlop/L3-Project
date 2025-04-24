// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/ZodiaqGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Networking/BaseGameInstance.h"
#include "Networking/SessionsManagerSubsystem.h"
#include "Networking/ZodiaqPlayerState.h"

FString AZodiaqGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
                                       const FString& Options, const FString& Portal)
{
	if (!USessionsManagerSubsystem::HasRunningSession)
		return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	
	const auto UUID = UGameplayStatics::ParseOption(Options, UBaseGameInstance::UUIDConnectOptionsKey);
	if (UUID.IsEmpty())
	{
		return "Given UUID is incorrect. Try to reconnect.";
	}

	// todo: This will allow player to join, but we need to check given UUID in the database afterwards,
	// and disconnect player if it is not found.

	const auto Name = UGameplayStatics::ParseOption(Options, UBaseGameInstance::UserNameConnectOptionsKey);
	if (Name.IsEmpty())
	{
		return "Given Name is incorrect. Try to reconnect.";
	}

	const auto PlayerState = NewPlayerController->GetPlayerState<AZodiaqPlayerState>();
	if (PlayerState == nullptr)
	{
		return "Server error. Try to reconnect.";
	}

	UE_LOG(LogTemp, Warning, TEXT("Player %s connected with UUID %s"), *Name, *UUID);
	PlayerState->ClientData = FClientData(UUID, Name);

	OnPlayerInitializedDelegate.Broadcast(PlayerState->ClientData);
	
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}
