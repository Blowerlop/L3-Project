#pragma once
#include "EffectStackingBehaviour.generated.h"

class UEffectDataAsset;

UCLASS(EditInlineNew, Blueprintable, Abstract)
class UEffectStackingBehaviour : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	float Stack(UEffectDataAsset* Effect, int Count);
};
