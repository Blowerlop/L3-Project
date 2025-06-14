#pragma once
#include "Vitals/InstigatorChain.h"
#include "EffectStackingBehaviour.generated.h"

class UEffectInstance;
struct FEffectInstancesGroup;
struct FEffectValueContainer;
class UEffectDataAsset;

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStackingBehaviourModifiers : uint8
{
	NONE = 0 UMETA(Hidden),
	Attack = 1 << 0 UMETA(DisplayName = "Attack"),
};
ENUM_CLASS_FLAGS(EStackingBehaviourModifiers);

UCLASS(EditInlineNew, Blueprintable, Abstract)
class UEffectStackingBehaviour : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = EStackingBehaviourModifiers), Category = "Params")
	int32 StackingModifierFlags = 0;
		
	UFUNCTION(BlueprintImplementableEvent)
	FEffectValueContainer Stack(UEffectDataAsset* Effect, FEffectInstancesGroup InstancesGroup);

protected:
	UFUNCTION(BlueprintCallable)
	bool IsModifierEnabled(EStackingBehaviourModifiers Multiplier) const
	{
		return StackingModifierFlags & static_cast<int32>(Multiplier);
	}
	
	UFUNCTION(BlueprintCallable)
	float GetModifiedValue(float BaseValue, AActor* Applier) const;

	UFUNCTION(BlueprintCallable)
	AActor* GetInstigatorChainOrigin(UPARAM(ref)const FInstigatorChain& InstigatorChain) const
	{
		return InstigatorChain.GetOriginAsActor();
	}
};
