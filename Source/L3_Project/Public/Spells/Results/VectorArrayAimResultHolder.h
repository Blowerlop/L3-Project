// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Results/AimResultHolder.h"
#include "VectorArrayAimResultHolder.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class L3_PROJECT_API UVectorArrayAimResultHolder : public UAimResultHolder
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> VectorArray;

	UFUNCTION(BlueprintCallable)
	void Set(TArray<FVector> InVectorArray);
};
