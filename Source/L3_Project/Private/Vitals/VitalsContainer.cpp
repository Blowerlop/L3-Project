#include "Vitals/VitalsContainer.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

FSrvOnVitalChanged UVitalsContainer::SrvOnVitalChanged{};

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

void UVitalsContainer::SrvAdd(const EVitalType Type, float Value, const FInstigatorChain& InstigatorChain)
{
	Value = GetModifiedValue(Type, Value, EVitalUpdateType::Add);
		
	SrvChangeValue(Type, Value, InstigatorChain);
		
	const auto InstigatorChainOrigin = InstigatorChain.GetOriginAsActor();
	ChangeValueMulticast(Type, Value, InstigatorChainOrigin);
}

void UVitalsContainer::SrvAddMultiple(const EVitalType Type, const TArray<FValueChainPair>& Values)
{
	for(const auto& [InValue, InstigatorChain] : Values)
	{
		const auto Value = GetModifiedValue(Type, InValue, EVitalUpdateType::Add);

		SrvChangeValue(Type, Value, InstigatorChain);

		const auto InstigatorChainOrigin = InstigatorChain.GetOriginAsActor();
		ChangeValueMulticast(Type, Value, InstigatorChainOrigin);
	}
}

void UVitalsContainer::SrvRemove(const EVitalType Type, float Value, const FInstigatorChain& InstigatorChain, const bool IgnoreModifiers)
{
	if (!IgnoreModifiers)
	{
		Value = GetModifiedValue(Type, Value, EVitalUpdateType::Remove);
	}

	const auto InstigatorChainOrigin = InstigatorChain.GetOriginAsActor();
		
	SrvChangeValue(Type, -Value, InstigatorChain);
	ChangeValueMulticast(Type, -Value, InstigatorChainOrigin);
}

void UVitalsContainer::SrvRemoveMultiple(const EVitalType Type, const TArray<FValueChainPair>& Values,
	bool IgnoreModifiers)
{
	for(const auto& [InValue, InstigatorChain] : Values)
	{
		auto Value = InValue;
		
		if (!IgnoreModifiers)
		{
			Value = GetModifiedValue(Type, InValue, EVitalUpdateType::Remove);
		}

		const auto InstigatorChainOrigin = InstigatorChain.GetOriginAsActor();
		
		SrvChangeValue(Type, -Value, InstigatorChain);
		ChangeValueMulticast(Type, -Value, InstigatorChainOrigin);
	}
}

void UVitalsContainer::ChangeValueMulticast_Implementation(const EVitalType Type, const float Value, AActor* InstigatorChainOrigin)
{
	if (UKismetSystemLibrary::IsServer(this)) return;
	
	if (FVital* Vital = Vitals.Find(Type))
	{
		const auto OldValue = Vital->Value;
		
		Vital->Value = FMath::Clamp(Vital->Value + Value, 0.0f, Vital->MaxValue);

		const auto Delta = Vital->Value - OldValue;

		const auto NewValue = Vital->Value;
		
		OnVitalChangedDelegate.Broadcast(Type, NewValue);
		OnVitalChangedWDeltaDelegate.Broadcast(Type, NewValue, Delta, InstigatorChainOrigin);
	}
}

void UVitalsContainer::SrvChangeValue(const EVitalType Type, const float Value, const FInstigatorChain& InstigatorChain)
{
	if (FVital* Vital = Vitals.Find(Type))
	{
		const auto OldValue = Vital->Value;
		
		Vital->Value = FMath::Clamp(Vital->Value + Value, 0.0f, Vital->MaxValue);

		const auto Delta = Vital->Value - OldValue;

		const auto NewValue = Vital->Value;
		
		OnVitalChangedDelegate.Broadcast(Type, NewValue);
		OnVitalChangedWDeltaDelegate.Broadcast(Type, NewValue, Delta, InstigatorChain.GetOriginAsActor());

		//UE_LOG(LogTemp, Error, TEXT("Vitals changed: %s"), *InstigatorChain.ToString());
		SrvOnVitalChanged.Broadcast(this, Type, NewValue, Delta, InstigatorChain);
		
		UpdateReplicatedVitals(Type, Vital);
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

