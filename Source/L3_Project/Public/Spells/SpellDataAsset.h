#pragma once

#include "SpellDataAsset.generated.h"

class UEffectDataAsset;
class USpellAimerParams;
class ASpellAimer;
class ASpell;

UCLASS(Blueprintable)
class L3_PROJECT_API USpellDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ASpellAimer> Aimer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	USpellAimerParams* AimerParams;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ASpell> Spell;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UEffectDataAsset*> Effects;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int Damage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int Heal;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Cooldown;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CanMove;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CanRotate;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* AnimationMontage;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int ComboIndex;
	
	// Combo
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bHasCombo;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2f ComboWindow;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USpellDataAsset* NextComboSpell;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetMontageSectionName() const;
	
#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
