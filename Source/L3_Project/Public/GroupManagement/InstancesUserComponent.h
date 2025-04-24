// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupableComponent.h"
#include "Components/ActorComponent.h"
#include "Networking/InstanceSettings.h"
#include "InstancesUserComponent.generated.h"

class UInstanceDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInstanceStartedDelegate, FInstanceSettings, Settings);

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
	void StartInstanceServerRPC(UInstanceDataAsset* InstanceDataAsset);

	UFUNCTION(Client, Reliable)
	void OnInstanceValidatedClientRPC(FServerInstanceSettings Settings);
	
	UFUNCTION(Client, Reliable)
	void OnInstanceStartedClientRPC(FInstanceSettings Settings);
	
	UFUNCTION(BlueprintCallable, Category = "Online Sessions")
	void StartInstance(UInstanceDataAsset* Asset);
	
	bool TryGetGroupableComponent(UGroupableComponent*& Out) const;
	static bool TryGetInstancesUser(const UGroupableComponent* GroupableComponent, UInstancesUserComponent*& Out);
};
