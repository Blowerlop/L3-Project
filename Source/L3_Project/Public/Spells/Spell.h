// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spell.generated.h"

class UEffectInstanceContainer;
class UEffectInstance;
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
	void Init(USpellDataAsset* DataAsset, AActor* SpellCaster, UAimResultHolder* Result, float duration);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SrvComplete();

	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly)
	void SrvOnComplete();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
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

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn="true"), Replicated)
	float Duration;

	UPROPERTY(BlueprintReadOnly)
	TMap<AActor*, UEffectInstanceContainer*> AppliedEffectsInstances;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bShouldStoreAppliedEffects = false;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool SrvApply(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SrvUnApply(AActor* Target);

private:
	UPROPERTY()
	FPrimaryAssetId SpellAssetId;

	virtual void OnSerializeNewActor(class FOutBunch& OutBunch) override;

	virtual void OnActorChannelOpen(class FInBunch& InBunch, class UNetConnection* Connection) override;
	
	void HandleSpellActions(UEffectable* Effectable) const;
};
