#pragma once
#include "EffectDataAsset.generated.h"

enum class EEffectValueType : uint8;
enum class EEffectType : uint8;

UCLASS(Blueprintable)
class L3_PROJECT_API UEffectDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EEffectType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUseDuration;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EEffectValueType, float> Values;

	UFUNCTION(BlueprintCallable)
	float GetValue(EEffectValueType ValueType) const;
	
#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
