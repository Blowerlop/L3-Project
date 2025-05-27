// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Results/AimResultHolder.h"
#include "ActorColorAimResultHolder.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class L3_PROJECT_API UActorColorAimResultHolder : public UAimResultHolder
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	AActor* Actor;
	
	UPROPERTY(BlueprintReadOnly)
	FColor Color;

	UFUNCTION(BlueprintCallable)
	void Set(AActor* InActor, FColor InColor);
};
