#include "Effects/Effectable.h"

#include "Effects/EffectDataAsset.h"
#include "Effects/EffectInstance.h"
#include "Effects/EffectSystemConfiguration.h"

void UEffectable::AddEffect(UEffectDataAsset* EffectData, AActor* Applier)
{
	const auto Container = GetEffectContainer(EffectData->Type);
	const auto Instance = NewObject<UEffectInstance>();

	Instance->Init(EffectData, Applier, this);
	Container->AddInstance(Instance);

	Refresh();
}

void UEffectable::RemoveEffect(UEffectInstance* Effect)
{
	const auto Container = GetEffectContainer(Effect->Data->Type);

	Container->RemoveInstance(Effect);

	Refresh();
}

UEffectInstanceContainer* UEffectable::GetEffectContainer(const EEffectType Type)
{
	if (!EffectsByType.Contains(Type))
	{
		EffectsByType.Add(Type, NewObject<UEffectInstanceContainer>());
	}
	
	return EffectsByType[Type];
}

void UEffectable::Refresh()
{
	TMap<UEffectDataAsset*, int> EffectCounts{};
	
	auto Types = TArray<EEffectType>();
	EffectsByType.GetKeys(Types);

	for(const auto& Type : Types)
	{
		EffectCounts.Empty();

		const auto Container = EffectsByType[Type];
		const auto Num = Container->GetNumInstances();

		for(int i = 0 ; i < Num; i++)
		{
			const auto Instance = Container->GetInstanceUnsafe(i);

			if (!EffectCounts.Contains(Instance->Data))
			{
				EffectCounts.Add(Instance->Data, 1);
			}
			else
			{
				EffectCounts[Instance->Data]++;
			}
		}

		TMap<UEffectDataAsset*, float> Values{};
		
		for(const auto& Pair : EffectCounts)
		{
			auto Effect = Pair.Key;
			const auto Count = Pair.Value;

			auto Value = HandleStacking(Effect, Count);
			Values.Add(Effect, Value);
		}

		const auto Resolver = GetResolver(Type);
		Resolver->Resolve(Values);
	}
}

float UEffectable::HandleStacking(UEffectDataAsset* Effect, int Count)
{
	// Do somtehing
	
	return 0.0f;
}

UEffectResolver* UEffectable::GetResolver(const EEffectType Type)
{
	if (!Resolvers.Contains(Type))
	{
		if (!Configuration->ResolversByType.Contains(Type))
		{
			UE_LOG(LogTemp, Error, TEXT("No resolver for type %d"), Type);
			return nullptr;
		}
		
		const auto [ResolverClass, Params] = Configuration->ResolversByType[Type];
		
		const auto Resolver = NewObject<UEffectResolver>(this, *ResolverClass);
		Resolver->Init(Params);
		
		Resolvers.Add(Type, Resolver);
		return Resolver;
	}

	return Resolvers[Type];
}
