#include "GroupManagement/GroupManager.h"
#include "GameFramework/PlayerState.h"
#include "GroupManagement/GroupableComponent.h"
#include "Networking/ZodiaqPlayerState.h"

TMap<int32, FServerGroupData> FGroupManager::Groups = TMap<int, FServerGroupData>();
int32 FGroupManager::GroupIdCounter = 0;

FString GetGroupMemberName(const UGroupableComponent* GroupMember)
{
	if (!IsValid(GroupMember)) return "Invalid Group Member";
	
	const auto Owner = GroupMember->GetOwner();
	if (!IsValid(Owner)) return "No Owner";

	const auto PlayerController = Cast<APlayerController>(Owner);
	if (!IsValid(PlayerController)) return Owner->GetName();

	return PlayerController->GetPlayerState<APlayerState>()->GetPlayerName();
}

bool GetGroupMemberClientData(const UGroupableComponent* GroupMember, FClientData& OutClientData)
{
	if (!IsValid(GroupMember)) return false;
	
	const auto Owner = GroupMember->GetOwner();
	if (!IsValid(Owner)) return false;

	const auto PlayerController = Cast<APlayerController>(Owner);
	if (!IsValid(PlayerController)) return false;
	
	const auto PlayerState = PlayerController->GetPlayerState<AZodiaqPlayerState>();
	if (!IsValid(PlayerState)) return false;

	OutClientData = PlayerState->ClientData;
	return true;
}

TArray<FString> FServerGroupData::GetMembersAsString() const
{
	TArray<FString> Members;
	Members.Reserve(GroupMembers.Num());

	for (const auto GroupMember : GroupMembers)
	{
		Members.Add(GetGroupMemberName(GroupMember));
	}
		
	return Members;
}

TArray<FClientData> FServerGroupData::GetMembersAsClientData() const
{
	TArray<FClientData> Members;
	Members.Reserve(GroupMembers.Num());

	for (const auto GroupMember : GroupMembers)
	{
		FClientData ClientData;

		if (!GetGroupMemberClientData(GroupMember, ClientData))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get client data for group member: %s"), *GetGroupMemberName(GroupMember));
			continue;
		}
		
		Members.Add(ClientData);
	}
	
	return Members;
}

int32 FGroupManager::CreateGroup(UGroupableComponent* Owner)
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

void FGroupManager::AddToGroup(UGroupableComponent* Player, const int32 GroupId)
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

void FGroupManager::RemoveFromGroup(UGroupableComponent* Player, const int32 GroupId)
{
	if (!IsValid(Player)) return;
	
	const auto Group = Groups.Find(GroupId);
	if (!Group) return;

	Group->RemoveMember(Player);
	Player->ReplicatedGroupData = FReplicatedGroupData { false, -1, {} };
	
	RefreshGroup(Group);
}

void FGroupManager::RemoveFromGroup(const UGroupableComponent* Player, const int32 GroupId)
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
	
	for (UGroupableComponent* GroupMember : Group->GroupMembers)
	{
		if (!IsValid(GroupMember)) continue;
		
		GroupMember->ReplicatedGroupData = FReplicatedGroupData { true, Group->GroupId, Members };
	}
}

void FGroupManager::DestroyGroup(const int32 GroupId)
{
	const auto Group = Groups.Find(GroupId);
	if (!Group) return;

	for (UGroupableComponent* GroupMember : Group->GroupMembers)
	{
		if (!IsValid(GroupMember)) continue;
		
		GroupMember->ReplicatedGroupData = FReplicatedGroupData { false, -1, {} };
	}
	
	Groups.Remove(GroupId);
}

bool HasInviteForGroup(UGroupableComponent* Player, const int GroupID)
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

void FGroupManager::InviteToGroup(UGroupableComponent* Inviter, UGroupableComponent* Invited)
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

void FGroupManager::AcceptGroupInvite(UGroupableComponent* Invited, int32 InviteId)
{
	if (!IsValid(Invited) || Invited->ReplicatedGroupData.IsValid)
		return;
	
	const auto Invite = Invited->ServerPendingInvites.Find(InviteId);
	if (!Invite) return;

	Invited->RemoveInvite(InviteId);
	AddToGroup(Invited, Invite->GroupId);
}

bool FGroupManager::IsGroupLeader(UGroupableComponent* Player)
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
