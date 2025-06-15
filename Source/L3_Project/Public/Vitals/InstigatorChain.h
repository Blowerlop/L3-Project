#pragma once
#include "IInstigatorChainElement.h"
#include "InstigatorChain.generated.h"

USTRUCT(BlueprintType)
struct FInstigatorChain
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<TScriptInterface<IInstigatorChainElement>> Chain{};

	/**
	 * Adds an element to the end of the chain.
	 * @param Element The element to add.
	 */
	FInstigatorChain& AddElement(const TScriptInterface<IInstigatorChainElement>& Element)
	{
		if (Element.GetObject())
		{
			Chain.Add(Element);
		}

		return *this;
	}

	AActor* GetOriginAsActor() const
	{
		return Chain.IsValidIndex(0) ? Cast<AActor>(Chain[0].GetObject()) : nullptr;
	}

	FString ToString() const
	{
		FString Result{};
		
		for (const auto& Element : Chain)
		{
			if (Element.GetObject())
			{
				Result += IInstigatorChainElement::Execute_GetIdentifier(Element.GetObject()) + TEXT(" -> ");
			}
		}

		return Result.IsEmpty() ? TEXT("Empty Chain") : Result;
	}
	
	FInstigatorChain() = default;

	FInstigatorChain(const FInstigatorChain& Other) = default;
	FInstigatorChain& operator=(const FInstigatorChain& Other) = default;
};
