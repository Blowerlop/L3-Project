// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellManager.generated.h"

class UAimResultHolder;
class USpellDataAsset;

UCLASS()
class L3_PROJECT_API ASpellManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpellManager();

	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
	UFUNCTION(BlueprintCallable)
	void TryCastSpellFromController(int SpellIndex, AActor* Caster, UAimResultHolder* Result) const;

	UFUNCTION(BlueprintCallable)
	void TryCastSpell(USpellDataAsset* SpellData, AActor* Caster, UAimResultHolder* Result) const;
};
