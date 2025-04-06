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
 
	UPROPERTY(BlueprintReadOnly)
	float BaseValue;

	UPROPERTY(BlueprintReadOnly)
	float ModFlat;

	UPROPERTY(BlueprintReadOnly)
	float ModCoef;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, EGameStatType, Type, float, Value);

// No need to have a replicated variable for stats, because player joins get default values, do things, server rpcs toward him.
// If plyaer disconnects, and reconnects, he gets default values again.
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class UStatsContainer : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UStatsContainer();

	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EGameStatType, FStat> Stats;

	UPROPERTY(BlueprintAssignable)
	FOnStatChangedDelegate OnStatChangedDelegate;
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetValue(const EGameStatType Type) const;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetMaxValue(const EGameStatType Type);

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetBaseValue(const EGameStatType Type);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddModFlat(const EGameStatType Type, const float ModFlat);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveModFlat(const EGameStatType Type, const float ModFlat);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveModFlatSilent(EGameStatType Type, float ModFlat);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddModCoef(const EGameStatType Type, const float ModCoef);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveModCoef(const EGameStatType Type, const float ModCoef);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveModCoefSilent(const EGameStatType Type, const float ModCoef);
	
private:
	void UpdateCurrentValue(const EGameStatType Type, FStat* Stat);

	void UpdateCurrentValue(const EGameStatType Type);

	UFUNCTION(Client, Reliable)
	void OnValueChangeRpc(const EGameStatType Type, const float Value);
};
