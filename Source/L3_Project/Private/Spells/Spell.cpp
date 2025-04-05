// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/Spell.h"

#include "Effects/Effectable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Spells/SpellDataAsset.h"

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

void ASpell::SrvApplyEffects(UEffectable* Target) const
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("ASpell::SrvApplyEffects called on client!"));
		return;
	}
	
	for(const auto Effect : Data->Effects)
	{
		Target->SrvAddEffect(Effect, Caster);
	}
}

