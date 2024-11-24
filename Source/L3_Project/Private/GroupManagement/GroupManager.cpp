#include "GroupManagement/GroupManager.h"
#include "GameFramework/PlayerState.h"
#include "Player/LobbyPlayerController.h"

TMap<int32, FServerGroupData> FGroupManager::Groups = TMap<int, FServerGroupData>();
int32 FGroupManager::GroupIdCounter = 0;

TArray<FString> FServerGroupData::GetMembersAsString() const
{
	TArray<FString> Members;
	Members.Reserve(GroupMembers.Num());

	for (const auto GroupMember : GroupMembers)
	{
		Members.Add(GroupMember->GetPlayerState<APlayerState>()->GetPlayerName());
	}
		
	return Members;
}

int32 FGroupManager::CreateGroup(ALobbyPlayerController* Owner)
{
	if (!IsValid(Owner)) return -1;
	
	if (Owner->ReplicatedGroupData.IsValid)
	{
		// Already has a group
		return -1;
	}

	GroupIdCounter++;
	
	auto data = FServerGroupData();
	data.GroupId = GroupIdCounter;
	
	Groups.Add(GroupIdCounter, data);

	AddToGroup(Owner, GroupIdCounter);
	return GroupIdCounter;
}

void FGroupManager::AddToGroup(ALobbyPlayerController* Player, const int32 GroupId)
{
	if (!IsValid(Player)) return;
	
	if (Player->ReplicatedGroupData.IsValid)
	{
		// Already in a group
		return;
	}
	
	const auto Group = Groups.Find(GroupId);
	if (!Group) return;

	Group->AddMember(Player);
	RefreshGroup(Group);
}

void FGroupManager::RemoveFromGroup(ALobbyPlayerController* Player, const int32 GroupId)
{
	if (!IsValid(Player)) return;
	
	const auto Group = Groups.Find(GroupId);
	if (!Group) return;

	Group->RemoveMember(Player);
	Player->ReplicatedGroupData = FReplicatedGroupData { false, -1, {} };
	
	RefreshGroup(Group);
}

void FGroupManager::RemoveFromGroup(const ALobbyPlayerController* Player, const int32 GroupId)
{
	if (!IsValid(Player)) return;
	
	const auto Group = Groups.Find(GroupId);
	if (!Group) return;

	Group->RemoveMember(Player);

	RefreshGroup(Group);
}

void FGroupManager::RefreshGroup(FServerGroupData* Group)
{
	if(Group->GroupMembers.Num() == 0)
	{
		DestroyGroup(Group->GroupId);
		return;
	}
	
	const auto Members = Group->GetMembersAsString();
	
	for (ALobbyPlayerController* GroupMember : Group->GroupMembers)
	{
		if (!IsValid(GroupMember)) continue;
		
		GroupMember->ReplicatedGroupData = FReplicatedGroupData { true, Group->GroupId, Members };
	}
}

void FGroupManager::DestroyGroup(const int32 GroupId)
{
	const auto Group = Groups.Find(GroupId);
	if (!Group) return;

	for (ALobbyPlayerController* GroupMember : Group->GroupMembers)
	{
		if (!IsValid(GroupMember)) continue;
		
		GroupMember->ReplicatedGroupData = FReplicatedGroupData { false, -1, {} };
	}
	
	Groups.Remove(GroupId);
}

bool HasInviteForGroup(ALobbyPlayerController* Player, const int GroupID)
{
	for (const auto& Invite : Player->ServerPendingInvites)
	{
		if (Invite.Value.GroupId == GroupID)
		{
			return true;
		}
	}

	return false;
}

void FGroupManager::InviteToGroup(ALobbyPlayerController* Inviter, ALobbyPlayerController* Invited)
{
	if (!IsValid(Inviter) || !IsValid(Invited)) return;

	if (Invited->ReplicatedGroupData.IsValid)
	{
		// Already in a group
		return;
	}
	
	int GroupId;
	
	if (!Inviter->ReplicatedGroupData.IsValid)
	{
		GroupId = CreateGroup(Inviter);
	}
	else
	{
		GroupId = Inviter->ReplicatedGroupData.GroupId;

		if (HasInviteForGroup(Invited, GroupId))
			return;
	}

	const auto Group = Groups.Find(GroupId);
	const auto ID = Invited->ServerInviteIdCounter++;
	
	Invited->AddInvite(FInviteData { GroupId, ID, Group->GetMembersAsString() });
}

void FGroupManager::AcceptGroupInvite(ALobbyPlayerController* Invited, int32 InviteId)
{
	UE_LOG(LogTemp, Log, TEXT("groupmanagher AcceptGroupInvite 1 %d"), InviteId);
	if (!IsValid(Invited) || Invited->ReplicatedGroupData.IsValid)
		return;
	
	UE_LOG(LogTemp, Log, TEXT("groupmanagher AcceptGroupInvite 2 %d"), InviteId);
	const auto Invite = Invited->ServerPendingInvites.Find(InviteId);
	if (!Invite) return;

	UE_LOG(LogTemp, Log, TEXT("groupmanagher AcceptGroupInvite 3 %d"), InviteId);
	Invited->RemoveInvite(InviteId);
	AddToGroup(Invited, Invite->GroupId);
}

bool FGroupManager::IsGroupLeader(ALobbyPlayerController* Player)
{
	return IsValid(Player)
	&& Player->ReplicatedGroupData.IsValid
	&& Groups.Contains(Player->ReplicatedGroupData.GroupId)
	&& Groups[Player->ReplicatedGroupData.GroupId].GroupMembers[0] == Player;
}

FServerGroupData* FGroupManager::GetGroup(const int32 GroupId)
{
	return Groups.Find(GroupId);
}
