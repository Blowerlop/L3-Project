#pragma once
#include "EffectResolver.generated.h"

class UEffectable;
class UEffectDataAsset;
class UEffectResolverParams;

UCLASS(Blueprintable, Abstract)
class UEffectResolver : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Init(UEffectable* Parent, UEffectResolverParams* Params);

	// Called effects change, only if there is at least one effect for this resolver
	// Need UPARAM(ref) to make sure Input is an input.
	UFUNCTION(BlueprintImplementableEvent)
	void Resolve(UPARAM(ref) TArray<float>& Input);

	// Called when there is no more effect for this resolver
	// Useful to remove stun or effects of this kind
	UFUNCTION(BlueprintImplementableEvent)
	void UnResolve();
};
