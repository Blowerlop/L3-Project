// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LobbyPlayerController.h"

#include "GameFramework/Character.h"
#include "Instances/InstanceDataAsset.h"
#include "Instances/InstanceDatabase.h"
#include "Net/UnrealNetwork.h"
#include "Networking/BaseGameInstance.h"
#include "Networking/InstancesManagerSubsystem.h"

class UInstanceDatabase;

void ALobbyPlayerController::AcceptGroupInviteServerRPC_Implementation(const int32 InviteId)
{
	UE_LOG(LogTemp, Log, TEXT("rpc accept invite %d"), InviteId);
	FGroupManager::AcceptGroupInvite(this, InviteId);
}

void ALobbyPlayerController::AddInvite(const FInviteData& Invite)
{
	ReplicatedPendingInvites.Add(Invite);
	ServerPendingInvites.Add(Invite.InviteId, Invite);
}

void ALobbyPlayerController::RemoveInvite(const int32 InviteId)
{
	if (!ServerPendingInvites.Contains(InviteId)) return;

	ServerPendingInvites.Remove(InviteId);
	ReplicatedPendingInvites.RemoveAll([InviteId](const FInviteData& Invite) { return Invite.InviteId == InviteId; });
}

void ALobbyPlayerController::InviteToGroup(ACharacter* Invited)
{
	InviteToGroupServerRpc(Invited);
}

void ALobbyPlayerController::InviteToGroupServerRpc_Implementation(ACharacter* Invited)
{
	const auto InviterController = Cast<ALobbyPlayerController>(this);
	const auto InvitedController = Cast<ALobbyPlayerController>(Invited->GetController());

	if (!IsValid(InviterController) || !IsValid(InvitedController))
	{
		UE_LOG(LogTemp, Error, TEXT("Can't invite to group invalid controller!"));
		return;
	}
	
	FGroupManager::InviteToGroup(InviterController, InvitedController);
}

void ALobbyPlayerController::AcceptGroupInvite(int32 InviteId)
{
	UE_LOG(LogTemp, Log, TEXT("AcceptGroupInvite %d"), InviteId);
	AcceptGroupInviteServerRPC(InviteId);
}

void ALobbyPlayerController::RefuseGroupInviteServerRPC_Implementation(int32 InviteId)
{
	RemoveInvite(InviteId);
}

void ALobbyPlayerController::RefuseGroupInvite(int32 InviteId)
{
	RefuseGroupInviteServerRPC(InviteId);
}

void ALobbyPlayerController::LeaveCurrentGroupServerRPC_Implementation()
{
	if (!ReplicatedGroupData.IsValid) return;

	FGroupManager::RemoveFromGroup(this, ReplicatedGroupData.GroupId);
}

void ALobbyPlayerController::LeaveCurrentGroup()
{
	LeaveCurrentGroupServerRPC();
}

void ALobbyPlayerController::OnInstanceValidatedClientRPC_Implementation(int32 InstanceID, int32 InstanceDataID)
{
	const auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
	if (!IsValid(GameInstance)) return;

	const auto InstancesManager = GameInstance->GetSubsystem<UInstancesManagerSubsystem>();
	if (!IsValid(InstancesManager)) return;

	const auto InstanceDatabase = GameInstance->GetSubsystem<UInstanceDatabase>();
	if (!IsValid(InstanceDatabase)) return;

	const auto Data = InstanceDatabase->GetInstanceDataAsset(InstanceDataID);
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find InstanceDataAsset with id %d!"), InstanceDataID);
		return;
	}
	
	InstancesManager->StartNewInstance(InstanceID, Data);
}

void ALobbyPlayerController::StartInstanceServerRPC_Implementation(int32 InstanceDataID)
{
	if (!FGroupManager::IsGroupLeader(this)) return;

	const auto NewInstanceId = UInstancesManagerSubsystem::GetNextInstanceID();
	const auto Group = FGroupManager::GetGroup(ReplicatedGroupData.GroupId);

	OnInstanceValidatedClientRPC(NewInstanceId, InstanceDataID);
	
	// Do not send this callback to the leader
	for (int32 i = 1; i < Group->GroupMembers.Num(); ++i)
	{
		const auto Member = Group->GroupMembers[i];
		if (!IsValid(Member)) continue;
		
		Member->OnInstanceStartedClientRPC(NewInstanceId, InstanceDataID);
	}
}

void ALobbyPlayerController::OnInstanceStartedClientRPC_Implementation(const int32 InstanceID, const int32 InstanceDataID)
{
	const auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
	if (!IsValid(GameInstance)) return;
	
	const auto InstanceDatabase = GameInstance->GetSubsystem<UInstanceDatabase>();
	if (!IsValid(InstanceDatabase)) return;

	const auto Data = InstanceDatabase->GetInstanceDataAsset(InstanceDataID);
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find InstanceDataAsset with id %d!"), InstanceDataID);
		return;
	}
	
	OnInstanceStartedDelegate.Broadcast(InstanceID, Data);
}

void ALobbyPlayerController::StartInstance(UInstanceDataAsset* Asset)
{
	StartInstanceServerRPC(Asset->AssetID);
}

void ALobbyPlayerController::OnRep_ReplicatedGroupData() const
{
	OnGroupChangedDelegate.Broadcast(ReplicatedGroupData);
}

void ALobbyPlayerController::OnRep_PendingInvites() const
{
	OnInvitesChangedDelegate.Broadcast(ReplicatedPendingInvites);
}

void ALobbyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ALobbyPlayerController, ReplicatedGroupData, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ALobbyPlayerController, ReplicatedPendingInvites, COND_OwnerOnly);
}
