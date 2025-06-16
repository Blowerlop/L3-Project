// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupManager.h"
#include "Components/ActorComponent.h"
#include "GroupableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroupChangedDelegate, FReplicatedGroupData, GroupData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvitesChangedDelegate, const TArray<FInviteData>&, Invites);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIsGroupLeaderResponse, bool, Value, FString, LeaderName);

UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class L3_PROJECT_API UGroupableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGroupableComponent();
	
	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedGroupData)
	FReplicatedGroupData ReplicatedGroupData{};

	TMap<int32, FInviteData> ServerPendingInvites{};
	int32 ServerInviteIdCounter{};
	
	void AddInvite(const FInviteData& Invite);
	void RemoveInvite(const int32 InviteId);

	UFUNCTION(BlueprintCallable, Category = "Groups")
	void InviteToGroup(UGroupableComponent* Invited);
    
	UFUNCTION(Server, Reliable)
	void InviteToGroupServerRpc(UGroupableComponent* Invited);
	
	UPROPERTY(BlueprintAssignable)
	FOnGroupChangedDelegate OnGroupChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInvitesChangedDelegate OnInvitesChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnIsGroupLeaderResponse OnTryOpenInstanceUIResponse;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void TryOpenInstanceUI();

	UFUNCTION(Client, Reliable)
	void TryOpenInstanceUIResponse(bool Value, const FString& LeaderName);
	
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
	
	UFUNCTION()
	void OnRep_ReplicatedGroupData() const;
	UFUNCTION()
	void OnRep_PendingInvites() const;

	void ReplicatePendingInvites();
};
