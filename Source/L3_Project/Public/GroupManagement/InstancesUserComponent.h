// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupableComponent.h"
#include "Components/ActorComponent.h"
#include "InstancesUserComponent.generated.h"

class UInstanceDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInstanceStartedDelegate, int32, InstanceId, UInstanceDataAsset*, InstanceDataAsset);

UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class L3_PROJECT_API UInstancesUserComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInstancesUserComponent();

	UPROPERTY(BlueprintAssignable)
	FOnInstanceStartedDelegate OnInstanceStartedDelegate;

	UFUNCTION(Client, Reliable)
	void ReturnToLobbyClientRPC();
	
private:
	UFUNCTION(Server, Reliable)
	void StartInstanceServerRPC(int32 InstanceDataID);

	UFUNCTION(Client, Reliable)
	void OnInstanceValidatedClientRPC(int32 InstanceID, int32 InstanceDataID);
	
	UFUNCTION(Client, Reliable)
	void OnInstanceStartedClientRPC(const int32 InstanceID, const int32 InstanceDataID);
	
	UFUNCTION(BlueprintCallable, Category = "Online Sessions")
	void StartInstance(UInstanceDataAsset* Asset);
	
	bool TryGetGroupableComponent(UGroupableComponent*& Out) const;
	static bool TryGetInstancesUser(const UGroupableComponent* GroupableComponent, UInstancesUserComponent*& Out);
};
