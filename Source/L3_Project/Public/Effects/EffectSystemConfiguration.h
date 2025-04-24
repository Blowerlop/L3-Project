#pragma once
#include "EffectResolver.h"
#include "EffectSystemConfiguration.generated.h"

class UEffectResolverParams;
enum class EEffectType : uint8;

USTRUCT(BlueprintType)
struct FEffectResolverData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UEffectResolver> ResolverClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced)
	UEffectResolverParams* Params{};
};

UCLASS(BlueprintType)
class UEffectSystemConfiguration : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EEffectType, FEffectResolverData> ResolversByType;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<TSubclassOf<UEffectResolver>, TSubclassOf<UEffectResolverParams>> ParamsByResolver;

	static bool NeedDuration(EEffectType Type);
	static bool NeedValue(EEffectType Type);
	static bool NeedRate(EEffectType Type);
	
#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
