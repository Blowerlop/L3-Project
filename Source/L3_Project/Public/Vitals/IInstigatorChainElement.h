#pragma once
#include "IInstigatorChainElement.generated.h"

UENUM()
enum class EInstigatorChainElementType : uint8
{
	Player,
	Boss,
	Spell,
	Effect
};

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
	EInstigatorChainElementType GetElementType();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FString GetIdentifier();
};
