// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutoAttackController.generated.h"


class UAimResultHolder;
class USpellDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, Abstract )
class L3_PROJECT_API UAutoAttackController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoAttackController();

protected:

	UPROPERTY(Blueprintable, EditAnywhere)
	USpellDataAsset* AutoAttackSpell;
	
	UFUNCTION(BlueprintCallable)
	void TriggerAutoAttack(UAimResultHolder* AimResultHolder);
	
private:

	UFUNCTION(Server, Reliable)
	void TriggerAutoAttackServerRpc_Vector(FVector Result);
	
};
