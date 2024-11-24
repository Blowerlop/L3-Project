// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/LobbyGameSession.h"

#include "GroupManagement/GroupManager.h"
#include "Player/LobbyPlayerController.h"

void ALobbyGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
	Super::UnregisterPlayer(ExitingPlayer);

	const auto ConstLobbyPlayer = Cast<ALobbyPlayerController>(ExitingPlayer);
	if (!IsValid(ConstLobbyPlayer))
	{
		return;
	}

	const auto LobbyPlayer = const_cast<ALobbyPlayerController*>(ConstLobbyPlayer);
	if (!IsValid(LobbyPlayer))
	{
		return;
	}
	
	FGroupManager::RemoveFromGroup(LobbyPlayer, LobbyPlayer->ReplicatedGroupData.GroupId);
}
