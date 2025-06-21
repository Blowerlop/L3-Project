// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Networking/BaseGameSession.h"
#include "InstanceGameSession.generated.h"

/**
 * 
 */
UCLASS()
class L3_PROJECT_API AInstanceGameSession : public ABaseGameSession
{
	GENERATED_BODY()
	
	virtual void HandleRegisterPlayerCompleted(FName EosSessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccessful) override;
	virtual void HandleUnregisterPlayerCompleted(FName EosSessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccessful) override;
};
