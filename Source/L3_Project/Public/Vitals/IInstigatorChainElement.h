#pragma once
#include "IInstigatorChainElement.generated.h"

UINTERFACE(BlueprintType, Blueprintable)
class UInstigatorChainElement : public UInterface
{
	GENERATED_BODY()
};

class IInstigatorChainElement
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FString GetIdentifier();
};
