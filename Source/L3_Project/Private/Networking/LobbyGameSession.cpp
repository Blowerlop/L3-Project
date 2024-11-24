// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/LobbyGameSession.h"

#include "GroupManagement/GroupManager.h"
#include "Player/LobbyPlayerController.h"

void ALobbyGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
	Super::UnregisterPlayer(ExitingPlayer);

	const auto LobbyPlayer = Cast<ALobbyPlayerController>(ExitingPlayer);
	if (!IsValid(LobbyPlayer))
	{
		return;
	}
	
	FGroupManager::RemoveFromGroup(LobbyPlayer, LobbyPlayer->ReplicatedGroupData.GroupId);
}
