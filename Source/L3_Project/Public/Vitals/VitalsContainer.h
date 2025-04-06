#pragma once
#include "VitalsContainer.generated.h"

class UStatsContainer;

UENUM(BlueprintType)
enum class EVitalType : uint8
{
	Health,
	Shield
};

UENUM(BlueprintType)
enum class EVitalUpdateType : uint8
{
	Add,
	Remove
};

USTRUCT(BlueprintType)
struct FVital
{
	GENERATED_BODY()

	FVital() : Value(0.0f), MaxValue(1.0f) {}
	FVital(const float InValue, const float MaxValue) : Value(InValue), MaxValue(MaxValue) {}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Value;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MaxValue;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVitalChangedDelegate, EVitalType, Type, float, Value);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class UVitalsContainer : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UVitalsContainer();

	UPROPERTY(BlueprintAssignable)
	FOnVitalChangedDelegate OnVitalChangedDelegate;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetValue(const EVitalType Type) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxValue(const EVitalType Type) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SrvAdd(const EVitalType Type, float Value);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SrvRemove(const EVitalType Type, float Value);

	UFUNCTION(NetMulticast, Reliable)
	void ChangeValueMulticast(const EVitalType Type, const float Value);
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EVitalType, FVital> Vitals;

	UFUNCTION(BlueprintImplementableEvent)
	float GetModifiedValue(const EVitalType Type, const float Value, const EVitalUpdateType UpdateType) const;
};
