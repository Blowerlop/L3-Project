// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Spells/SpellController.h"
#include "AutoAttackController.generated.h"

class UAimResultHolder;
class USpellDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackStateChanged, USpellDataAsset*, Spell, bool, IsAttacking);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, Abstract )
class L3_PROJECT_API UAutoAttackController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoAttackController();

	UPROPERTY(ReplicatedUsing=OnCurrentAttackSpellChanged, EditAnywhere, BlueprintReadWrite)
	USpellDataAsset* CurrentAttackSpell;

	UPROPERTY(BlueprintReadOnly)
	bool IsAttacking = false;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttackStateChanged OnAttackStateChanged;

	UFUNCTION(BlueprintCallable)
	void OnAttackAnimEnded();
	
	void StartAttack(USpellDataAsset* Spell);
	void StopAttack();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UPROPERTY(Blueprintable, EditAnywhere)
	USpellDataAsset* AutoAttackSpell;
	
	UFUNCTION(BlueprintCallable)
	void TriggerAutoAttack(UAimResultHolder* AimResultHolder);
	
private:

	UFUNCTION(Server, Reliable)
	void TriggerAutoAttackServerRpc_Vector(FVector Result);

	UFUNCTION()
	void OnCurrentAttackSpellChanged();
};
