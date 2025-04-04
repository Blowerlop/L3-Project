#include "Stats/StatsContainer.h"

UStatsContainer::UStatsContainer()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UStatsContainer::BeginPlay()
{
	Super::BeginPlay();

	for(auto& [Type, Stat] : Stats)
	{
		Stat.BaseValue = Stat.Value;
	}
}

float UStatsContainer::GetValue(const EGameStatType Type) const
{
	if (const FStat* Stat = Stats.Find(Type))
	{
		return Stat->Value;
	}
		
	return 1.0f;
}

float UStatsContainer::GetMaxValue(const EGameStatType Type)
{
	if (const FStat* Stat = Stats.Find(Type))
	{
		return Stat->MaxValue;
	}
		
	return 1.0f;
}

float UStatsContainer::GetBaseValue(const EGameStatType Type)
{
	if (const FStat* Stat = Stats.Find(Type))
	{
		return Stat->BaseValue;
	}
		
	return 1.0f;
}

void UStatsContainer::AddModFlat(const EGameStatType Type, const float ModFlat)
{
	if (FStat* Stat = Stats.Find(Type))
	{
		Stat->ModFlat += ModFlat;
		UpdateCurrentValue(Type, Stat);
	}
}

void UStatsContainer::RemoveModFlat(const EGameStatType Type, const float ModFlat)
{
	if (FStat* Stat = Stats.Find(Type))
	{
		Stat->ModFlat -= ModFlat;
		UpdateCurrentValue(Type, Stat);
	}
}

void UStatsContainer::RemoveModFlatSilent(const EGameStatType Type, const float ModFlat)
{
	if (FStat* Stat = Stats.Find(Type))
	{
		Stat->ModFlat -= ModFlat;
	}
}

void UStatsContainer::AddModCoef(const EGameStatType Type, const float ModCoef)
{
	if (FStat* Stat = Stats.Find(Type))
	{
		Stat->ModCoef += ModCoef;
		UpdateCurrentValue(Type, Stat);
	}
}

void UStatsContainer::RemoveModCoef(const EGameStatType Type, const float ModCoef)
{
	if (FStat* Stat = Stats.Find(Type))
	{
		Stat->ModCoef -= ModCoef;
		UpdateCurrentValue(Type, Stat);
	}
}

void UStatsContainer::RemoveModCoefSilent(const EGameStatType Type, const float ModCoef)
{
	if (FStat* Stat = Stats.Find(Type))
	{
		Stat->ModCoef -= ModCoef;
	}
}

void UStatsContainer::UpdateCurrentValue(const EGameStatType Type, FStat* Stat)
{
	// First apply coef (BaseValue * 1.15f => 15% bonus) then add flat (BaseValue * 1.15f + 10 => +10 bonus)

	const auto ModCoef = FMath::Clamp(Stat->ModCoef, 0.0f, 9999);
	const auto Value = FMath::Clamp(Stat->BaseValue * ModCoef + Stat->ModFlat, 0, Stat->MaxValue);
	Stat->Value = Value;
	
	const APlayerController* OwningPC = Cast<APlayerController>(GetOwner()->GetOwner());

	if (OwningPC && !OwningPC->IsLocalController())
	{
		OnValueChangeRpc(Type, Value);
	}

	UE_LOG(LogTemp, Warning, TEXT("Stat %d updated to %f with coef %f and flat %f"), (int)Type, Stat->Value, ModCoef, Stat->ModFlat);
}

void UStatsContainer::UpdateCurrentValue(const EGameStatType Type)
{
	if (FStat* Stat = Stats.Find(Type))
	{
		UpdateCurrentValue(Type, Stat);
	}
}

void UStatsContainer::OnValueChangeRpc_Implementation(const EGameStatType Type, const float Value)
{
	if (FStat* Stat = Stats.Find(Type))
	{
		Stat->Value = Value;
		UE_LOG(LogTemp, Warning, TEXT("Stat %d updated to %f"), (int)Type, Stat->Value);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Stat %d not found"), (int)Type);
	}
}
