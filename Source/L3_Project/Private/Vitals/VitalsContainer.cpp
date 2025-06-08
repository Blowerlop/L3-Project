#include "Vitals/VitalsContainer.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

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

float UVitalsContainer::GetMaxValue_NotPure(EVitalType Type)
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

		if (UKismetSystemLibrary::IsServer(this))
		{
			UpdateReplicatedVitals(Type, Vital);
		}
	}
}

void UVitalsContainer::UpdateReplicatedVitals(const EVitalType Type, const FVital* Vital)
{
	const auto RepVital = ReplicatedVitals.FindByPredicate([Type](const FReplicatedVital& X)
	{
		return X.Type == Type;
	});

	if (RepVital == nullptr)
	{
		ReplicatedVitals.Add(FReplicatedVital { Type, Vital->Value, Vital->MaxValue });
	}
	else
	{
		RepVital->Value = Vital->Value;
	}
}

void UVitalsContainer::OnInitialVitalsRep()
{
	for(const auto& RepVital : ReplicatedVitals)
	{
		const auto Vital = Vitals.Find(RepVital.Type);

		if (Vital == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Vital %s not found in local map"), *UEnum::GetDisplayValueAsText(RepVital.Type).ToString());
			return;
		}

		Vital->Value = RepVital.Value;
		Vital->MaxValue = RepVital.MaxValue;
	}
}

void UVitalsContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UVitalsContainer, ReplicatedVitals, COND_InitialOnly);
}

