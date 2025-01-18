// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/LobbyGameSession.h"

#include "GroupManagement/GroupableComponent.h"
#include "GroupManagement/GroupManager.h"

void ALobbyGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
	Super::UnregisterPlayer(ExitingPlayer);

	const auto GroupMember = ExitingPlayer->GetComponentByClass<UGroupableComponent>();
	if (!IsValid(GroupMember))
	{
		return;
	}
	
	FGroupManager::RemoveFromGroup(GroupMember, GroupMember->ReplicatedGroupData.GroupId);
}

FName ALobbyGameSession::GetOnlineSubsystemName() const
{
	return "EOS";
}
