// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/Spell.h"

#include "Effects/Effectable.h"
#include "Effects/EffectType.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Spells/SpellDataAsset.h"
#include "Spells/SpellDatabase.h"
#include "Stats/StatsContainer.h"
#include "Vitals/IAliveState.h"
#include "Vitals/VitalsContainer.h"

ASpell::ASpell()
{
	PrimaryActorTick.bCanEverTick = UKismetSystemLibrary::IsServer(this);
}

void ASpell::Init(USpellDataAsset* DataAsset, AActor* SpellCaster, UAimResultHolder* Result)
{
	Data = DataAsset;
	Caster = SpellCaster;
	CasterController = Cast<AController>(SpellCaster->GetOwner());
	AimResult = Result;
}

bool ASpell::SrvApply(AActor* Target)
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::SrvApplyEffects called on client!"));
		return false;
	}

	if (const auto AliveState = Cast<IAliveState>(Target); AliveState != nullptr)
	{
		if (!IAliveState::Execute_GetIsAlive(Target))
		{
			return false;
		}
	}
	
	bool IsValid = false;
	
	if (const auto Vitals = Target->GetComponentByClass<UVitalsContainer>(); Vitals != nullptr)
	{
		Vitals->SrvAdd(EVitalType::Health, Data->Heal);

		auto Damage = Data->Damage;
		
		if(const auto Stats = Caster->GetComponentByClass<UStatsContainer>(); Stats != nullptr)
		{
			Damage *= Stats->GetValue(EGameStatType::Attack);
		}
		
		Vitals->SrvRemove(EVitalType::Health, Damage);
		
		IsValid = true;
	}

	if (const auto Effectable = Target->GetComponentByClass<UEffectable>(); Effectable != nullptr)
	{
		HandleSpellActions(Effectable);
		
		if (bShouldStoreAppliedEffects)
		{
			if (!AppliedEffectsInstances.Contains(Target))
			{
				AppliedEffectsInstances.Add(Target, NewObject<UEffectInstanceContainer>());
			}

			Effectable->SrvAddEffectsWithBuffer(Data->Effects, Caster, AppliedEffectsInstances[Target]->Instances);
		}
		else
		{
			Effectable->SrvAddEffects(Data->Effects, Caster);
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

void ASpell::OnSerializeNewActor(class FOutBunch& OutBunch)
{
	Super::OnSerializeNewActor(OutBunch);
	
	OutBunch << Data->AssetID;
}

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

