#pragma once
#include "EffectInstance.generated.h"

class UEffectable;
class UEffectDataAsset;

UCLASS(Blueprintable)
class UEffectInstance : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(UEffectDataAsset* EffectAsset, AActor* EffectApplier, UEffectable* EffectParent);

	void Stop();

	virtual void BeginDestroy() override;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	UEffectDataAsset* Data;

	UPROPERTY(BlueprintReadOnly)
	UEffectable* Parent;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* Applier;
	
	UFUNCTION(BlueprintImplementableEvent)
	void Init_Internal();

	UFUNCTION(BlueprintImplementableEvent)
	void Stop_Internal();
	
private:
	FTimerHandle LifetimeTimerHandle{};

	void OnTimerEnded();
};
