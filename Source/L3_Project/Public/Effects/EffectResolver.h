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
	void Init(UEffectable* Parent, UEffectResolverParams* Params);

	// Need UPARAM(ref) to make sure Input is an input.
	UFUNCTION(BlueprintImplementableEvent)
	void Resolve(UPARAM(ref) TArray<float>& Input);
};
