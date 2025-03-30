#include "Vitals/VitalsContainer.h"

#include "Stats/StatsContainer.h"

float UVitalsContainer::GetValue(const EVitalType Type) const
{
	if (const FVital* Vital = Vitals.Find(Type))
	{
		return Vital->Value;
	}
		
	return 0;
}

float UVitalsContainer::GetMaxValue(const EVitalType Type) const
{
	if (const FVital* Vital = Vitals.Find(Type))
	{
		return Vital->MaxValue;
	}
		
	return 0;
}

void UVitalsContainer::Add(const EVitalType Type, float Value)
{
	Value = GetModifiedValue(Type, Value, EVitalUpdateType::Add);
	
	if (FVital* Vital = Vitals.Find(Type))
	{
		Vital->Value = FMath::Clamp(Vital->Value + Value, 0.0f, Vital->MaxValue);
	}
}

void UVitalsContainer::Remove(const EVitalType Type, float Value)
{
	Value = GetModifiedValue(Type, Value, EVitalUpdateType::Remove);

	if (FVital* Vital = Vitals.Find(Type))
	{
		Vital->Value = FMath::Clamp(Vital->Value - Value, 0.0f, Vital->MaxValue);
	}
}
