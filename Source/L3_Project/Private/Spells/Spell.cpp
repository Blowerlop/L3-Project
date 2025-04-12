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
	
	Init_Internal();

	bIsInit = true;
}

bool ASpell::SrvApply(AActor* Target) const
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
		for(const auto Effect : Data->Effects)
		{
			Effectable->SrvAddEffect(Effect, Caster);
		}

		IsValid = true;
	}

	return IsValid;
}

