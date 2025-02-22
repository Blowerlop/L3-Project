// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellManager.generated.h"

class USpellController;
class UAutoAttackController;
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
	void RequestSpellCastFromController(int SpellIndex, USpellController* SpellController, UAimResultHolder* Result) const;

	UFUNCTION(BlueprintCallable)
	void RequestAttack(UAutoAttackController* AttackController, UAimResultHolder* Result, double ClientTime) const;
	
	UFUNCTION(BlueprintCallable)
	void TryCastSpell(USpellDataAsset* SpellData, AActor* Caster, UAimResultHolder* Result) const;

	static bool IsInComboWindow(const USpellDataAsset* Spell, double ClientTime, double StartTime, double EndTime);
};
