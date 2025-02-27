// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Spells/SpellController.h"
#include "AutoAttackController.generated.h"

class UAimResultHolder;
class USpellDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackStartDelegate, USpellDataAsset*, AttackSpell);

UCLASS(Blueprintable)
class UControllerAttackState : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool IsAttacking{};
	
	UPROPERTY(BlueprintReadOnly)
	USpellDataAsset* AttackSpell{};

	// Valid only on server side
	UPROPERTY(BlueprintReadOnly)
	UAimResultHolder* AimResult{};

	UPROPERTY(BlueprintReadOnly)
	double AnimationStartTime{};

	UPROPERTY(BlueprintReadOnly)
	double AnimationEndTime{};

	UPROPERTY(BlueprintReadOnly)
	int ComboIndex{};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, Abstract )
class L3_PROJECT_API UAutoAttackController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoAttackController();

	UPROPERTY(BlueprintReadOnly)
	UControllerAttackState* AttackState;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttackStartDelegate OnAttackStart;
	
	void SendAttackResponse(UAimResultHolder* Result, USpellDataAsset* Spell);
	void SendAttackResponse(UAimResultHolder* Result);
	
	UFUNCTION(NetMulticast, Reliable)
	void AttackResponseMultiCastRpc(USpellDataAsset* Spell);
	
	UFUNCTION(BlueprintCallable)
	void SrvOnAnimationAttackSpellNotify();
	
	UFUNCTION(BlueprintCallable)
	void OnAnimationEndedNotify();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAttacking() const;
	
protected:

	UPROPERTY(Blueprintable, EditAnywhere)
	USpellDataAsset* AutoAttackSpell;
	
	UFUNCTION(BlueprintCallable)
	void RequestAutoAttack(UAimResultHolder* AimResultHolder);
	
private:

	virtual void BeginPlay() override;
	
	UFUNCTION(Server, Reliable)
	void RequestAutoAttackServerRpc_Vector(FVector Result, double ClientTime);
};
