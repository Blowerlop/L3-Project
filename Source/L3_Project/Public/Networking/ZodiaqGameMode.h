// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstanceSettings.h"
#include "GameFramework/GameModeBase.h"
#include "ZodiaqGameMode.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInitialized, FClientData, ClientData);

UCLASS()
class L3_PROJECT_API AZodiaqGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerInitialized OnPlayerInitializedDelegate;

protected:
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
};
