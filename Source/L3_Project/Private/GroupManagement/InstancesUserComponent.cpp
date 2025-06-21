// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupManagement/InstancesUserComponent.h"

#include "GroupManagement/GroupManager.h"
#include "Instances/InstanceDataAsset.h"
#include "Instances/InstanceDatabase.h"
#include "Networking/BaseGameInstance.h"
#include "Networking/InstanceSettings.h"
#include "Networking/InstancesManagerSubsystem.h"


// Sets default values for this component's properties
UInstancesUserComponent::UInstancesUserComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UInstancesUserComponent::ReturnToLobbyClientRPC_Implementation()
{
	const auto GameInstance = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (!IsValid(GameInstance)) return;

	const auto InstancesManager = GameInstance->GetSubsystem<UInstancesManagerSubsystem>();
	if (!IsValid(InstancesManager)) return;

	InstancesManager->ReturnToLobby();
}

void UInstancesUserComponent::StartInstance(UInstanceDataAsset* Asset)
{
	StartInstanceServerRPC(Asset);
}

void UInstancesUserComponent::OnInstanceStartedClientRPC_Implementation(FInstanceSettings Settings)
{
	const auto GameInstance = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (!IsValid(GameInstance)) return;
	
	const auto InstanceDatabase = GameInstance->GetSubsystem<UInstanceDatabase>();
	if (!IsValid(InstanceDatabase)) return;
	
	OnInstanceStartedDelegate.Broadcast(Settings);
}

void UInstancesUserComponent::OnInstanceValidatedClientRPC_Implementation(FServerInstanceSettings Settings)
{
	const auto GameInstance = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (!IsValid(GameInstance)) return;

	const auto InstancesManager = GameInstance->GetSubsystem<UInstancesManagerSubsystem>();
	if (!IsValid(InstancesManager)) return;

	const auto InstanceDatabase = GameInstance->GetSubsystem<UInstanceDatabase>();
	if (!IsValid(InstanceDatabase)) return;
	
	InstancesManager->StartNewInstance(Settings);
}

void UInstancesUserComponent::StartInstanceServerRPC_Implementation(UInstanceDataAsset* InstanceDataAsset)
{
	UGroupableComponent* GroupableComponent = nullptr;
	if (!TryGetGroupableComponent(GroupableComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Can't start instance because there is no GroupableComponent on InstancesUser Owner!"));
		return;
	}

	if (!GroupableComponent->ReplicatedGroupData.IsValid)
	{
		FGroupManager::CreateGroup(GroupableComponent);
	}
	
	if (!FGroupManager::IsGroupLeader(GroupableComponent)) return;

	const auto SessionId = UInstancesManagerSubsystem::GetNextInstanceID();
	const auto Group = FGroupManager::GetGroup(GroupableComponent->ReplicatedGroupData.GroupId);

	const FServerInstanceSettings HostSettings = {
		InstanceDataAsset,
		SessionId,
		Group->GetMembersAsClientData()
	};
	
	OnInstanceValidatedClientRPC(HostSettings);

	const FInstanceSettings ClientSettings = {
		InstanceDataAsset,
		SessionId,
		Group->GetAsReplicatedData()
	};
	
	// Do not send this callback to the leader
	for (int32 i = 1; i < Group->GroupMembers.Num(); ++i)
	{
		const auto Member = Group->GroupMembers[i];
		if (!IsValid(Member)) continue;
		
		UInstancesUserComponent* InstancesUser = nullptr;
		if(!TryGetInstancesUser(Member, InstancesUser))
		{
			UE_LOG(LogTemp, Error, TEXT("GroupMember at index %d has no InstancesUserComponent!"), i);
			continue;
		}
		
		InstancesUser->OnInstanceStartedClientRPC(ClientSettings);
	}
}

bool UInstancesUserComponent::TryGetGroupableComponent(UGroupableComponent*& Out) const
{
	Out = nullptr;
	
	const auto Owner = GetOwner();
	if (!IsValid(Owner)) return false;

	Out = Owner->GetComponentByClass<UGroupableComponent>();
	return IsValid(Out);
}

bool UInstancesUserComponent::TryGetInstancesUser(const UGroupableComponent* GroupableComponent, UInstancesUserComponent*& Out)
{
	Out = nullptr;
	if (!IsValid(GroupableComponent)) return false;
	
	const auto Owner = GroupableComponent->GetOwner();
	if (!IsValid(Owner)) return false;

	Out = Owner->GetComponentByClass<UInstancesUserComponent>();
	return IsValid(Out);
}


