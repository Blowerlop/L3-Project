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
	if (FVital* Vital = Vitals.Find(Type))
	{
		Value = GetModifiedValue(Type, Value, EVitalUpdateType::Add);
		
		Vital->Value = FMath::Clamp(Vital->Value + Value, 0.0f, Vital->MaxValue);

		UE_LOG(LogTemp, Warning, TEXT("Added %f to %s, new value: %f"), Value, *UEnum::GetDisplayValueAsText(Type).ToString(), Vital->Value);
	}
}

void UVitalsContainer::Remove(const EVitalType Type, float Value)
{
	if (FVital* Vital = Vitals.Find(Type))
	{
		Value = GetModifiedValue(Type, Value, EVitalUpdateType::Remove);
		
		Vital->Value = FMath::Clamp(Vital->Value - Value, 0.0f, Vital->MaxValue);

		UE_LOG(LogTemp, Warning, TEXT("Removed %f from %s, new value: %f"), Value, *UEnum::GetDisplayValueAsText(Type).ToString(), Vital->Value);
	}
}
