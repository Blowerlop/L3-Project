// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GroupManagement/GroupManager.h"
#include "L3_Project/L3_ProjectPlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */

class UInstanceDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroupChangedDelegate, FReplicatedGroupData, GroupData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvitesChangedDelegate, const TArray<FInviteData>&, Invites);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInstanceStartedDelegate, int32, InstanceId, UInstanceDataAsset*, InstanceDataAsset);

UCLASS()
class L3_PROJECT_API ALobbyPlayerController : public AL3_ProjectPlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedGroupData)
	FReplicatedGroupData ReplicatedGroupData{};

	TMap<int32, FInviteData> ServerPendingInvites{};
	int32 ServerInviteIdCounter{};
	
	void AddInvite(const FInviteData& Invite);
	void RemoveInvite(const int32 InviteId);

	UFUNCTION(BlueprintCallable, Category = "Groups")
	void InviteToGroup(ACharacter* Invited);
    
    UFUNCTION(Server, Reliable)
	void InviteToGroupServerRpc(ACharacter* Invited);

	UPROPERTY(BlueprintAssignable)
	FOnGroupChangedDelegate OnGroupChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInvitesChangedDelegate OnInvitesChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInstanceStartedDelegate OnInstanceStartedDelegate;

private:
	UPROPERTY(ReplicatedUsing=OnRep_PendingInvites)
	TArray<FInviteData> ReplicatedPendingInvites{};
	
	UFUNCTION(Server, Reliable)
	void AcceptGroupInviteServerRPC(int32 InviteId);

	UFUNCTION(BlueprintCallable, Category = "Groups")
	void AcceptGroupInvite(int32 InviteId);

	UFUNCTION(Server, Reliable)
	void RefuseGroupInviteServerRPC(int32 InviteId);
	
	UFUNCTION(BlueprintCallable, Category = "Groups")
	void RefuseGroupInvite(int32 InviteId);

	UFUNCTION(Server, Reliable)
	void LeaveCurrentGroupServerRPC();
	
	UFUNCTION(BlueprintCallable, Category = "Groups")
	void LeaveCurrentGroup();

	UFUNCTION(Server, Reliable)
	void StartInstanceServerRPC(int32 InstanceDataID);

	UFUNCTION(Client, Reliable)
	void OnInstanceValidatedClientRPC(int32 InstanceID, int32 InstanceDataID);
	
	UFUNCTION(Client, Reliable)
	void OnInstanceStartedClientRPC(const int32 InstanceID, const int32 InstanceDataID);
	
	UFUNCTION(BlueprintCallable, Category = "Online Sessions")
	void StartInstance(UInstanceDataAsset* Asset);
	
	UFUNCTION()
	void OnRep_ReplicatedGroupData() const;
	UFUNCTION()
	void OnRep_PendingInvites() const;
};
