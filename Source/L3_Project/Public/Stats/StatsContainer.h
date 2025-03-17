#pragma once

USTRUCT(BlueprintType)
struct FStat
{
	GENERATED_BODY()
	
	FStat() : Value(0.0f), MaxValue(1.0f), BaseValue(0.0f) {}
	FStat(const float InValue, const float MaxValue) : Value(InValue), MaxValue(MaxValue), BaseValue(InValue) {}
	
	UPROPERTY(BlueprintReadOnly)
	float Value;

	UPROPERTY(BlueprintReadOnly)
	float MaxValue;

	UPROPERTY(BlueprintReadOnly)
	float BaseValue;
};

UENUM(BlueprintType)
enum class EGameStatType : uint8
{
	Health,
	Attack,
	Defense,
	MoveSpeed,
};

UCLASS(Blueprintable)
class UStatsContainer : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EGameStatType, FStat> Stats;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetValue(const EGameStatType Type) const
	{
		if (const FStat* Stat = Stats.Find(Type))
		{
			return Stat->Value;
		}
		
		return 1.0f;
	}
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetMaxValue(const EGameStatType Type)
	{
		if (const FStat* Stat = Stats.Find(Type))
		{
			return Stat->MaxValue;
		}
		
		return 1.0f;
	}
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetBaseValue(const EGameStatType Type)
	{
		if (const FStat* Stat = Stats.Find(Type))
		{
			return Stat->BaseValue;
		}
		
		return 1.0f;
	}
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float Add(const EGameStatType Type, const float Value)
	{
		if (FStat* Stat = Stats.Find(Type))
		{
			const auto CurrentValue = Stat->Value;
			Stat->Value += Value;
			
			return Stat->Value - CurrentValue;
		}
		
		return 0.0f;
	}

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float Subtract(const EGameStatType Type, const float Value)
	{
		if (FStat* Stat = Stats.Find(Type))
		{
			const auto CurrentValue = Stat->Value;
			Stat->Value -= Value;
			
			return CurrentValue - Stat->Value;
		}
		
		return 0.0f;
	}
};
