// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InstancePlayerController.h"

#include "Networking/InstancesManagerSubsystem.h"

void AInstancePlayerController::ReturnToLobbyClientRPC_Implementation()
{
	const auto GameInstance = GetGameInstance();
	if (!IsValid(GameInstance)) return;

	const auto InstancesManager = GameInstance->GetSubsystem<UInstancesManagerSubsystem>();
	if (!IsValid(InstancesManager)) return;

	InstancesManager->ReturnToLobby();
}
