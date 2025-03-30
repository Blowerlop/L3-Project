#pragma once
#include "StatsContainer.generated.h"

UENUM(BlueprintType)
enum class EGameStatType : uint8
{
	Attack,
	Defense,
	MoveSpeed,
	HealBonus
};

USTRUCT(BlueprintType)
struct FStat
{
	GENERATED_BODY()
	
	FStat() : Value(0.0f), MaxValue(1.0f), BaseValue(0.0f), ModFlat(0), ModCoef(1) {}

	FStat(const float InValue, const float MaxValue)
	: Value(InValue), MaxValue(MaxValue), BaseValue(InValue), ModFlat(0), ModCoef(1) {}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Value;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MaxValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BaseValue;

	UPROPERTY(BlueprintReadOnly)
	float ModFlat;

	UPROPERTY(BlueprintReadOnly)
	float ModCoef;
};

USTRUCT()
struct FReplicatedStat
{
	GENERATED_BODY()

	UPROPERTY()
	EGameStatType Type;
	
	UPROPERTY()
	float Value;
};

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class UStatsContainer : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UStatsContainer();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EGameStatType, FStat> Stats;
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetValue(const EGameStatType Type) const;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetMaxValue(const EGameStatType Type);

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetBaseValue(const EGameStatType Type);
	
	UFUNCTION(BlueprintCallable)
	void AddModFlat(const EGameStatType Type, const float ModFlat);

	UFUNCTION(BlueprintCallable)
	void RemoveModFlat(const EGameStatType Type, const float ModFlat);

	UFUNCTION(BlueprintCallable)
	void RemoveModFlatSilent(EGameStatType Type, float ModFlat);

	UFUNCTION(BlueprintCallable)
	void AddModCoef(const EGameStatType Type, const float ModCoef);

	UFUNCTION(BlueprintCallable)
	void RemoveModCoef(const EGameStatType Type, const float ModCoef);

	UFUNCTION(BlueprintCallable)
	void RemoveModCoefSilent(const EGameStatType Type, const float ModCoef);
	
private:
	void UpdateCurrentValue(const EGameStatType Type, FStat* Stat);

	void UpdateCurrentValue(const EGameStatType Type);

	UFUNCTION(Client, Reliable)
	void OnValueChangeRpc(const EGameStatType Type, const float Value);
};
