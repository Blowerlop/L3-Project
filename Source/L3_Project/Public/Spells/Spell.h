// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spell.generated.h"

class UVitalsContainer;
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
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn="true"))
	AController* CasterController;
	
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn="true"))
	AActor* Caster;
	
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn="true"))
	USpellDataAsset* Data;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn="true"))
	UAimResultHolder* AimResult;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool SrvApply(AActor* Target);
};
