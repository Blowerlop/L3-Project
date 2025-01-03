// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/Spell.h"

#include "Kismet/KismetSystemLibrary.h"

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

void ASpell::TryApply()
{
	TryApply_Internal();
}

