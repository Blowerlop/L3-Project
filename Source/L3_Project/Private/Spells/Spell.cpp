// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/Spell.h"

#include "Effects/Effectable.h"
#include "Effects/EffectType.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Spells/SpellDataAsset.h"
#include "Spells/SpellDatabase.h"
#include "Stats/StatsContainer.h"
#include "Vitals/IAliveState.h"
#include "Vitals/VitalsContainer.h"
#include "Vitals/InstigatorChain.h"

ASpell::ASpell()
{
	PrimaryActorTick.bCanEverTick = UKismetSystemLibrary::IsServer(this);
}

void ASpell::Init(USpellDataAsset* DataAsset, AActor* SpellCaster, UAimResultHolder* Result, float duration)
{
	Data = DataAsset;
	Caster = SpellCaster;
	CasterController = Cast<AController>(SpellCaster->GetOwner());
	AimResult = Result;
	Duration = duration;
}

void ASpell::SrvComplete()
{
	SrvOnComplete();
}

void ASpell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpell, Caster);
	DOREPLIFETIME(ASpell, Duration);
}

FString ASpell::GetIdentifier_Implementation()
{
	return FString::Printf(TEXT("%d"), Data->AssetID);
}

bool ASpell::SrvApply(AActor* Target)
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::SrvApplyEffects called on client!"));
		return false;
	}

	if (!Caster)
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::SrvApply - Caster is null!"));
		return false;
	}

	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::SrvApply - Target is null!"));
		return false;
	}

	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::SrvApply - Data is null!"));
		return false;
	}

	if (const auto Vitals = Caster->GetComponentByClass<UVitalsContainer>(); Vitals != nullptr)
	{
		if (Vitals->GetValue(EVitalType::Health) <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASpell::SrvApply - Caster is dead!"));
			return false;
		}
	}

	if (const auto AliveState = Cast<IAliveState>(Target); AliveState != nullptr)
	{
		if (!IAliveState::Execute_GetIsAlive(Target))
		{
			return false;
		}
	}
	
	bool IsValid = false;

	auto InstigatorChain = GetInstigatorChain();
	
	if (const auto Vitals = Target->GetComponentByClass<UVitalsContainer>(); Vitals != nullptr)
	{
		if (Data->Heal != 0) Vitals->SrvAdd(EVitalType::Health, Data->Heal, InstigatorChain);

		auto Damage = Data->Damage;
		auto TrueDamage = Data->TrueDamage;
		
		if(const auto Stats = Caster->GetComponentByClass<UStatsContainer>(); Stats != nullptr)
		{
			const auto AttackStat = Stats->GetValue(EGameStatType::Attack);
			
			Damage *= AttackStat;
			TrueDamage *= AttackStat;
		}

		if (Damage != 0) Vitals->SrvRemove(EVitalType::Health, Damage, InstigatorChain);
		if (TrueDamage != 0) Vitals->SrvRemove(EVitalType::Health, TrueDamage, InstigatorChain, /*ignoreModifiers:*/ true);
		
		IsValid = true;
	}

	if (const auto Effectable = Target->GetComponentByClass<UEffectable>(); Effectable != nullptr)
	{
		HandleSpellActions(Effectable);

		if (Data->Effects.Num() == 0) return true;
		
		if (bShouldStoreAppliedEffects)
		{
			if (!AppliedEffectsInstances.Contains(Target))
			{
				AppliedEffectsInstances.Add(Target, NewObject<UEffectInstanceContainer>());
			}

			Effectable->SrvAddEffectsWithBuffer(Data->Effects, InstigatorChain, AppliedEffectsInstances[Target]->Instances);
		}
		else
		{
			Effectable->SrvAddEffects(Data->Effects, InstigatorChain);
		}

		IsValid = true;
	}

	return IsValid;
}

void ASpell::SrvUnApply(AActor* Target)
{
	if (!bShouldStoreAppliedEffects)
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::SrvUnApply called without storing effects! "
							  "You should set bShouldStoreAppliedEffects to true!"));
		return;
	}

	if (!AppliedEffectsInstances.Contains(Target))
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::SrvUnApply AppliedEffectsInstances does not contain target! "
							  "You should set bShouldStoreAppliedEffects to true!"));
		return;
	}
	
	if (const auto Effectable = Target->GetComponentByClass<UEffectable>(); Effectable != nullptr)
	{
		const auto Container = AppliedEffectsInstances[Target];

		Effectable->SrvRemoveEffects(Container->Instances);

		AppliedEffectsInstances[Target]->Instances.Empty();
	}
}

// Serialized on server
void ASpell::OnSerializeNewActor(class FOutBunch& OutBunch)
{
	Super::OnSerializeNewActor(OutBunch);

	// This actor is map placed and not instantiated. This is not something we managed
	if (bNetStartup)
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell:: Map placed ! Remove it from the map and only instantiate it"));
		return;
	}

	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::OnSerializeNewActor - Data is null!"));
		return;
	}

	OutBunch << Data->AssetID;
}

// Deserialized on client, load spell data asset
void ASpell::OnActorChannelOpen(class FInBunch& InBunch, class UNetConnection* Connection)
{
	Super::OnActorChannelOpen(InBunch, Connection);

	uint8 ID{};
	InBunch << ID;

	const auto Database = GetGameInstance()->GetSubsystem<USpellDatabase>();
	Data = Database->DataAssets.FindRef(ID);

	if(!IsValid(Data))
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::OnActorChannelOpen Failed to load spell data asset!"));
	}
}

void ASpell::HandleSpellActions(UEffectable* Effectable) const
{
	const auto Actions = Data->SpellActions;

	if (Actions & static_cast<int32>(ESpellAction::Cleanse))
	{
		Effectable->Cleanse();
	}
	
	if (Actions & static_cast<int32>(ESpellAction::Debuff))
	{
		Effectable->Debuff();
	}
}

FInstigatorChain ASpell::GetInstigatorChain()
{
	auto Chain = FInstigatorChain();
	
	if (Caster->Implements<UInstigatorChainElement>())
	{
		Chain.AddElement(TScriptInterface<IInstigatorChainElement>(Caster));
	}
	
	Chain.AddElement(TScriptInterface<IInstigatorChainElement>(this));
	return Chain;
}
