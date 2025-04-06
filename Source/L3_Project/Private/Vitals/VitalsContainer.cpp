#include "Vitals/VitalsContainer.h"

UVitalsContainer::UVitalsContainer()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

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

void UVitalsContainer::SrvAdd(const EVitalType Type, float Value)
{
	if (FVital* Vital = Vitals.Find(Type))
	{
		Value = GetModifiedValue(Type, Value, EVitalUpdateType::Add);
		
		ChangeValueMulticast(Type, Value);

		UE_LOG(LogTemp, Warning, TEXT("Added %f to %s, new value: %f"), Value, *UEnum::GetDisplayValueAsText(Type).ToString(), Vital->Value);
	}
}

void UVitalsContainer::SrvRemove(const EVitalType Type, float Value)
{
	if (FVital* Vital = Vitals.Find(Type))
	{
		Value = GetModifiedValue(Type, Value, EVitalUpdateType::Remove);

		ChangeValueMulticast(Type, -Value);
		
		UE_LOG(LogTemp, Warning, TEXT("Removed %f from %s, new value: %f"), Value, *UEnum::GetDisplayValueAsText(Type).ToString(), Vital->Value);
	}
}

void UVitalsContainer::ChangeValueMulticast_Implementation(const EVitalType Type, const float Value)
{
	if (FVital* Vital = Vitals.Find(Type))
	{
		Vital->Value = FMath::Clamp(Vital->Value + Value, 0.0f, Vital->MaxValue);
		OnVitalChangedDelegate.Broadcast(Type, Vital->Value);
	}
}
