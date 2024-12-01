// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/InstanceGameSession.h"

#include "Networking/BaseGameInstance.h"
#include "Networking/InstancesManagerSubsystem.h"


void AInstanceGameSession::HandleRegisterPlayerCompleted(FName EosSessionName, const TArray<FUniqueNetIdRef>& PlayerIds,
                                                         bool bWasSuccessful)
{
	Super::HandleRegisterPlayerCompleted(EosSessionName, PlayerIds, bWasSuccessful);
}

void AInstanceGameSession::HandleUnregisterPlayerCompleted(FName EosSessionName,
	const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccessful)
{
	Super::HandleUnregisterPlayerCompleted(EosSessionName, PlayerIds, bWasSuccessful);
	
	if (RegisteredPlayerCount == 1)
	{
		if (!UInstancesManagerSubsystem::IsInstanceBeingDestroyed) return;

		const auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
		if (!GameInstance) return;
		
		const auto InstancesManager = GameInstance->GetSubsystem<UInstancesManagerSubsystem>();
		if (!InstancesManager) return;
		
		InstancesManager->ReturnToLobby();
	}
}

FName AInstanceGameSession::GetOnlineSubsystemName() const
{
	return UInstancesManagerSubsystem::GetOnlineSubsystemName();
}
