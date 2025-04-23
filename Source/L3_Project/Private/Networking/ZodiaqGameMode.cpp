// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/ZodiaqGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Networking/BaseGameInstance.h"
#include "Networking/ZodiaqPlayerState.h"

FString AZodiaqGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
                                       const FString& Options, const FString& Portal)
{
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

	PlayerState->ClientData = FClientData(UUID, Name);
	
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}
