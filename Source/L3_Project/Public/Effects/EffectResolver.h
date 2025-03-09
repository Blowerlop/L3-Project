#pragma once
#include "EffectResolver.generated.h"

class UEffectDataAsset;
class UEffectResolverParams;

UCLASS(Blueprintable, Abstract)
class UEffectResolver : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Init(UEffectResolverParams* Params);

	UFUNCTION(BlueprintImplementableEvent)
	void Resolve(TMap<UEffectDataAsset*, float>& Input);
};
