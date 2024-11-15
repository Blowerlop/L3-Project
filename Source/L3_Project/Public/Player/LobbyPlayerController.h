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
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Groups")
	void OnGroupChanged(FReplicatedGroupData Group);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Groups")
	void OnInvitesChanged(const TArray<FInviteData>& Invites);

	UFUNCTION(BlueprintImplementableEvent, Category = "Online Sessions")
	void OnInstanceStarted(int32 InstanceID);
	
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
	void StartInstanceServerRPC();

	UFUNCTION(Client, Reliable)
	void OnInstanceValidatedClientRPC(int32 InstanceID);
	
	UFUNCTION(Client, Reliable)
	void OnInstanceStartedClientRPC(int32 InstanceID);
	
	UFUNCTION(BlueprintCallable, Category = "Online Sessions")
	void StartInstance();
	
	UFUNCTION()
	void OnRep_ReplicatedGroupData();
	UFUNCTION()
	void OnRep_PendingInvites();
};
