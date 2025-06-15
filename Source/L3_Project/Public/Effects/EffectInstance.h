#pragma once
#include "Vitals/IInstigatorChainElement.h"
#include "Vitals/InstigatorChain.h"
#include "EffectInstance.generated.h"

class UEffectable;
class UEffectDataAsset;

UCLASS(BlueprintType)
class UEffectInstance : public UObject, public IInstigatorChainElement
{
	GENERATED_BODY()
	
public:
	void Init(UEffectDataAsset* EffectAsset, const FInstigatorChain& InInstigatorChain, UEffectable* EffectParent);

	virtual void BeginDestroy() override;

	UPROPERTY(BlueprintReadOnly)
	FGuid InstanceID;
	
	UPROPERTY(BlueprintReadOnly)
	UEffectDataAsset* Data;

	UPROPERTY(BlueprintReadOnly)
	UEffectable* Parent;
	
	UPROPERTY(BlueprintReadOnly)
	FInstigatorChain InstigatorChain;

	void Release();

	virtual FString GetIdentifier_Implementation() override;

	virtual EInstigatorChainElementType GetElementType_Implementation() override
	{
		return EInstigatorChainElementType::Effect;
	}
	
private:
	FTimerHandle LifetimeTimerHandle{};

	void OnTimerEnded();
};
