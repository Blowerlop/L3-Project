// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/Spell.h"

#include "Effects/Effectable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Spells/SpellDataAsset.h"
#include "Vitals/VitalsContainer.h"

ASpell::ASpell()
{
	PrimaryActorTick.bCanEverTick = UKismetSystemLibrary::IsServer(this);
}

void ASpell::Init(USpellDataAsset* SpellData, AActor* SpellCaster, UAimResultHolder* Result)
{
	Data = SpellData;
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

	bool IsValid = false;
	
	if (const auto Vitals = Target->GetComponentByClass<UVitalsContainer>(); Vitals != nullptr)
	{
		Vitals->SrvAdd(EVitalType::Health, Data->Heal);
		Vitals->SrvRemove(EVitalType::Health, Data->Damage);
		IsValid = true;
	}

	if (const auto Effectable = Target->GetComponentByClass<UEffectable>(); Effectable != nullptr)
	{
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

