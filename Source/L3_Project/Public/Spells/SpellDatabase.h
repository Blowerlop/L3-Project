// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SpellDatabase.generated.h"

class USpellDataAsset;
/**
 * 
 */
UCLASS(Blueprintable)
class L3_PROJECT_API USpellDatabase : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Spell Database")
	TMap<uint8, USpellDataAsset*> DataAssets;

	UFUNCTION(BlueprintCallable)
	TArray<USpellDataAsset*> GroupByConditionSpell(TArray<USpellDataAsset*> InSpells);
};
