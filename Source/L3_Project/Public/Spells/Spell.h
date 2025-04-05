// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spell.generated.h"

class UEffectable;
class UAimResultHolder;
class USpellDataAsset;

UCLASS()
class L3_PROJECT_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:
	ASpell();
	
	UFUNCTION(BlueprintCallable)
	void Init(USpellDataAsset* SpellData, AActor* SpellCaster, UAimResultHolder* Result);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EObjectTypeQuery> ApplyObjectType;

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bIsInit = false;

	UPROPERTY(BlueprintReadOnly)
	AController* CasterController;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* Caster;
	
	UPROPERTY(BlueprintReadOnly)
	USpellDataAsset* Data;

	UPROPERTY(BlueprintReadOnly)
	UAimResultHolder* AimResult;

	UFUNCTION(BlueprintImplementableEvent)
	void Init_Internal();

	UFUNCTION(BlueprintCallable)
	void SrvApplyEffects(UEffectable* Target) const;
};
