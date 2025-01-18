// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Results/AimResultHolder.h"
#include "VectorAimResultHolder.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class L3_PROJECT_API UVectorAimResultHolder : public UAimResultHolder
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FVector Vector;

	UFUNCTION(BlueprintCallable)
	void Set(FVector InVector);
};
