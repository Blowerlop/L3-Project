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

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class UVitalsContainer : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetValue(const EVitalType Type) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxValue(const EVitalType Type) const;

	UFUNCTION(BlueprintCallable)
	void Add(const EVitalType Type, float Value);

	UFUNCTION(BlueprintCallable)
	void Remove(const EVitalType Type, float Value);
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EVitalType, FVital> Vitals;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStatsContainer* StatsContainer;

	UFUNCTION(BlueprintImplementableEvent)
	float GetModifiedValue(const EVitalType Type, const float Value, const EVitalUpdateType UpdateType) const;
};
