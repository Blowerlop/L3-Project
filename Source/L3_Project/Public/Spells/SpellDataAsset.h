#pragma once

#include "SpellDataAsset.generated.h"

enum class EEffectImpact : uint8;
class UEffectDataAsset;
class USpellAimerParams;
class ASpellAimer;
class ASpell;

UCLASS(Blueprintable)
class L3_PROJECT_API USpellDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 AssetID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpellDataAsset* WeaponSpellCondition;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = ESpellAction))
	int32 SpellActions = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Desc;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* Icon;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Cooldown;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EEffectImpact SpellImpact;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CanMove;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CanRotate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsInstantAiming;
	
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

	float GetMontageSectionLength(int Index) const;
	
#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
