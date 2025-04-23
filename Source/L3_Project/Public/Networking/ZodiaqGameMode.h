// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZodiaqGameMode.generated.h"

/**
 * 
 */
UCLASS()
class L3_PROJECT_API AZodiaqGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
};
