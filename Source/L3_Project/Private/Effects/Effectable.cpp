#include "Effects/Effectable.h"

#include "Effects/EffectDataAsset.h"
#include "Effects/EffectInstance.h"
#include "Effects/EffectStackingBehaviour.h"
#include "Effects/EffectSystemConfiguration.h"
#include "Effects/EffectType.h"
#include "Kismet/KismetSystemLibrary.h"

UEffectable::UEffectable()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UEffectInstance* UEffectable::SrvAddEffect(UEffectDataAsset* EffectData, AActor* Applier)
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectable::SrvAddEffect called on client!"));
		return nullptr;
	}
	
	const auto Container = GetEffectContainer(EffectData->Type);
	const auto Instance = NewObject<UEffectInstance>();

	Instance->Init(EffectData, Applier, this);
	Container->AddInstance(Instance);

	Refresh();
	return Instance;
}

void UEffectable::SrvAddEffects(const TArray<UEffectDataAsset*>& Effects, AActor* Applier)
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectable::SrvAddEffect called on client!"));
		return;
	}
	
	for(auto& Effect : Effects)
	{
		if (Effect == nullptr) continue;
		
		const auto Container = GetEffectContainer(Effect->Type);
		const auto Instance = NewObject<UEffectInstance>();

		Instance->Init(Effect, Applier, this);
		Container->AddInstance(Instance);
	}

	Refresh();
}

void UEffectable::SrvAddEffectsWithBuffer(UPARAM(ref) const TArray<UEffectDataAsset*>& Effects, AActor* Applier, TArray<UEffectInstance*>& OutAppliedEffects)
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectable::SrvAddEffect called on client!"));
		return;
	}
	
	OutAppliedEffects.Empty();
	
	for(auto& Effect : Effects)
	{
		if (Effect == nullptr) continue;
		
		const auto Container = GetEffectContainer(Effect->Type);
		const auto Instance = NewObject<UEffectInstance>();

		Instance->Init(Effect, Applier, this);
		Container->AddInstance(Instance);
		OutAppliedEffects.Add(Instance);
	}

	Refresh();
}

void UEffectable::SrvRemoveEffect(UEffectInstance* Effect)
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectable::SrvRemoveEffect called on client!"));
		return;
	}
	
	const auto Container = GetEffectContainer(Effect->Data->Type);

	Container->RemoveInstance(Effect);

	Effect->Release();
	
	Refresh();
}

void UEffectable::SrvRemoveEffects(const TArray<UEffectInstance*>& Effects)
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectable::SrvRemoveEffect called on client!"));
		return;
	}

	for(auto& Effect : Effects)
	{
		if (Effect == nullptr) continue;
		
		const auto Container = GetEffectContainer(Effect->Data->Type);
		Container->RemoveInstance(Effect);

		Effect->Release();
	}

	Refresh();
}

void UEffectable::Cleanse()
{
	RemoveEffectsBuffer.Empty();
	
	for(auto& [_, Container] : EffectsByType)
	{
		for(int i = Container->GetNumInstances() - 1; i >= 0; i--)
		{
			const auto Instance = Container->GetInstanceUnsafe(i);
			if (Instance->Data->Impact == EEffectImpact::Bad)
			{
				RemoveEffectsBuffer.Add(Instance);
			}
		}
	}

	SrvRemoveEffects(RemoveEffectsBuffer);
}

void UEffectable::Debuff()
{
	RemoveEffectsBuffer.Empty();
	
	for(auto& [_, Container] : EffectsByType)
	{
		for(int i = Container->GetNumInstances(); i >= 0; i--)
		{
			const auto Instance = Container->GetInstanceUnsafe(i);
			if (Instance->Data->Impact == EEffectImpact::Good)
			{
				RemoveEffectsBuffer.Add(Instance);
			}
		}
	}

	SrvRemoveEffects(RemoveEffectsBuffer);
}

UEffectInstanceContainer* UEffectable::GetEffectContainer(const EEffectType Type)
{
	if (!EffectsByType.Contains(Type))
	{
		EffectsByType.Add(Type, NewObject<UEffectInstanceContainer>());
	}
	
	return EffectsByType[Type];
}

// This will count appearances of each effect data asset in the container
void CountEffects(TMap<UEffectDataAsset*, int>& EffectCounts, const UEffectInstanceContainer* Container)
{
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
}

void UEffectable::Refresh()
{
	TMap<UEffectDataAsset*, int> EffectCounts{};
	
	TMap<UEffectDataAsset*, float> Values{};
	
	auto Types = TArray<EEffectType>();
	EffectsByType.GetKeys(Types);

	for(const auto& Type : Types)
	{
		const auto Container = EffectsByType[Type];
		
		if (Container->GetNumInstances() == 0)
		{
			if (ActiveResolvers.Contains(Type))
			{
				ResolversCache[Type]->UnResolve();
				
				ActiveResolvers.Remove(Type);
			}
			
			continue;
		}
	
		Values.Empty();

		// Effects like stun or root don't have value. Don't need to do all this
		if (UEffectSystemConfiguration::NeedValue(Type))
		{
			EffectCounts.Empty();
			
			CountEffects(EffectCounts, Container);
			
			GetValueForEachEffect(Values, EffectCounts);
		}

		const auto Resolver = GetResolver(Type);
		Resolver->Resolve(Values);

		if (!ActiveResolvers.Contains(Type))
			ActiveResolvers.Add(Type);
	}
}

void UEffectable::GetValueForEachEffect(TMap<UEffectDataAsset*, float>& ValuesBuffer, TMap<UEffectDataAsset*, int>& EffectCounts)
{
	for(const auto& Pair : EffectCounts)
	{
		const auto Effect = Pair.Key;
		const auto Count = Pair.Value;

		auto Value = Effect->StackingBehaviour->Stack(Effect, Count);
		ValuesBuffer.Add(Effect, Value);
	}
}

UEffectResolver* UEffectable::GetResolver(const EEffectType Type)
{
	if (!ResolversCache.Contains(Type))
	{
		if (!Configuration->ResolversByType.Contains(Type))
		{
			UE_LOG(LogTemp, Error, TEXT("No resolver for type %d"), Type);
			return nullptr;
		}
		
		const auto [ResolverClass, Params] = Configuration->ResolversByType[Type];
		
		const auto Resolver = NewObject<UEffectResolver>(this, *ResolverClass);
		Resolver->Init(this, Params);
		
		ResolversCache.Add(Type, Resolver);
		return Resolver;
	}

	return ResolversCache[Type];
}

void UEffectable::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for(const auto Type : ActiveResolvers)
	{
		const auto Resolver = ResolversCache[Type];
		if (!Resolver->bNeedTicking) continue;
		
		Resolver->Tick(DeltaTime);
	}
}
