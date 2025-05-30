// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Results/AimResultHolder.h"
#include "VectorColorIntAimResultHolder.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class L3_PROJECT_API UVectorColorIntAimResultHolder : public UAimResultHolder
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FVector Vector;

	UPROPERTY(BlueprintReadOnly)
	FColor Color;

	UPROPERTY(BlueprintReadOnly)
	int32 Int;

	UFUNCTION(BlueprintCallable)
	void Set(FVector InVector, FColor InColor, int32 InInt);
};
