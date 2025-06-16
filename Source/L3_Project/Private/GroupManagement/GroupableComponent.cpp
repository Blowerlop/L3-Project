// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupManagement/GroupableComponent.h"

#include "Net/UnrealNetwork.h"

UGroupableComponent::UGroupableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UGroupableComponent::AddInvite(const FInviteData& Invite)
{
	ReplicatedPendingInvites.Add(Invite);
	ServerPendingInvites.Add(Invite.InviteId, Invite);

	ReplicatePendingInvites();
}

void UGroupableComponent::RemoveInvite(const int32 InviteId)
{
	if (!ServerPendingInvites.Contains(InviteId)) return;

	ServerPendingInvites.Remove(InviteId);
	ReplicatedPendingInvites.RemoveAll([InviteId](const FInviteData& Invite) { return Invite.InviteId == InviteId; });

	ReplicatePendingInvites();
}

void UGroupableComponent::InviteToGroup(UGroupableComponent* Invited)
{
	InviteToGroupServerRpc(Invited);
}

void UGroupableComponent::TryOpenInstanceUI_Implementation()
{
	if (!ReplicatedGroupData.IsValid)
	{
		FGroupManager::CreateGroup(this);
		TryOpenInstanceUIResponse(true, FString());
		return;
	}

	const auto Group = FGroupManager::GetGroup(ReplicatedGroupData.GroupId);

	if (Group == nullptr)
	{
		TryOpenInstanceUIResponse(false, FString());
		return;
	}
	
	const auto Members = Group->GetMembersAsString();
	if (Members.Num() == 0)
	{
		TryOpenInstanceUIResponse(false, FString());
		return;
	}
	
	TryOpenInstanceUIResponse(FGroupManager::IsGroupLeader(this), Members[0]);
}

void UGroupableComponent::TryOpenInstanceUIResponse_Implementation(bool Value, const FString& LeaderName)
{
	OnTryOpenInstanceUIResponse.Broadcast(Value, LeaderName);
}

void UGroupableComponent::AcceptGroupInvite(int32 InviteId)
{
	AcceptGroupInviteServerRPC(InviteId);
}

void UGroupableComponent::RefuseGroupInvite(int32 InviteId)
{
	RefuseGroupInviteServerRPC(InviteId);
}

void UGroupableComponent::LeaveCurrentGroup()
{
	LeaveCurrentGroupServerRPC();
}

void UGroupableComponent::OnRep_ReplicatedGroupData() const
{
	OnGroupChangedDelegate.Broadcast(ReplicatedGroupData);
}

void UGroupableComponent::OnRep_PendingInvites() const
{
	OnInvitesChangedDelegate.Broadcast(ReplicatedPendingInvites);
}

void UGroupableComponent::LeaveCurrentGroupServerRPC_Implementation()
{
	if (!ReplicatedGroupData.IsValid) return;

	FGroupManager::RemoveFromGroup(this, ReplicatedGroupData.GroupId);
}

void UGroupableComponent::RefuseGroupInviteServerRPC_Implementation(int32 InviteId)
{
	RemoveInvite(InviteId);
}

void UGroupableComponent::AcceptGroupInviteServerRPC_Implementation(int32 InviteId)
{
	FGroupManager::AcceptGroupInvite(this, InviteId);
}

void UGroupableComponent::InviteToGroupServerRpc_Implementation(UGroupableComponent* Invited)
{
	FGroupManager::InviteToGroup(this, Invited);
}

void UGroupableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UGroupableComponent, ReplicatedGroupData, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGroupableComponent, ReplicatedPendingInvites, COND_OwnerOnly);
}

void UGroupableComponent::ReplicatePendingInvites()
{
	ReplicatedPendingInvites = ReplicatedPendingInvites;
}

