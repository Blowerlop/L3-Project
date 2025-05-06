// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellsIdRegenerator.generated.h"

/**
 * 
 */
UCLASS()
class L3_PROJECT_API USpellsIdRegenerator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	static void MarkAssetDirty(UObject* Asset)
	{
		if (Asset)
		{
			Asset->MarkPackageDirty();
		}
	}
};
