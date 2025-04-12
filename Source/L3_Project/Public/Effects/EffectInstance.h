#pragma once
#include "EffectInstance.generated.h"

class UEffectable;
class UEffectDataAsset;

UCLASS(BlueprintType)
class UEffectInstance : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(UEffectDataAsset* EffectAsset, AActor* EffectApplier, UEffectable* EffectParent);

	virtual void BeginDestroy() override;
	
	UPROPERTY(BlueprintReadOnly)
	UEffectDataAsset* Data;

	UPROPERTY(BlueprintReadOnly)
	UEffectable* Parent;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* Applier;
	
private:
	FTimerHandle LifetimeTimerHandle{};

	void OnTimerEnded();
};
