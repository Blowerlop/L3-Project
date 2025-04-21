// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Results/AimResultHolder.h"
#include "ActorAimResultHolder.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class L3_PROJECT_API UActorAimResultHolder : public UAimResultHolder
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	AActor* Actor;

	UFUNCTION(BlueprintCallable)
	void Set(AActor* InActor);
};
