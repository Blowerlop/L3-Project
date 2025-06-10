#pragma once
#include "EffectDataAsset.generated.h"

enum class EEffectImpact : uint8;
class UEffectStackingBehaviour;
enum class EEffectStackingType : uint8;
enum class EEffectValueType : uint8;
enum class EEffectType : uint8;

UCLASS(Blueprintable)
class L3_PROJECT_API UEffectDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UTexture2D* Icon;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EEffectType Type;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EEffectImpact Impact;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bUseDuration;
	
	UPROPERTY(BlueprintReadOnly, Instanced, EditAnywhere, meta = (EditCondition = "bNeedStacking", EditConditionHides))
	UEffectStackingBehaviour* StackingBehaviour;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EEffectValueType, float> Values;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetValue(EEffectValueType ValueType) const;

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY()
	bool bNeedStacking;
#endif
	
#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
