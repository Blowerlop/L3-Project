#include "Stats/StatsContainer.h"

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
	Stat->Value = FMath::Clamp(Stat->BaseValue * ModCoef + Stat->ModFlat, 0, Stat->MaxValue);

	UE_LOG(LogTemp, Warning, TEXT("Stat %d updated to %f with coef %f and flat %f"), (int)Type, Stat->Value, ModCoef, Stat->ModFlat);
}

void UStatsContainer::UpdateCurrentValue(const EGameStatType Type)
{
	if (FStat* Stat = Stats.Find(Type))
	{
		UpdateCurrentValue(Type, Stat);
	}
}
