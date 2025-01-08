#pragma once

#include "SpellDataAsset.generated.h"

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Cooldown;

#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
