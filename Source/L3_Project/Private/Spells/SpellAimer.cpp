// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/SpellAimer.h"

ASpellAimer::ASpellAimer()
{ 
	PrimaryActorTick.bCanEverTick = true;
}

void ASpellAimer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsAiming) return;
	
	MoveToOwner();

	Update_Internal();
}

void ASpellAimer::Init(USceneComponent* Socket)
{
	OwnerSocket = Socket;

	Init_Internal();
}

void ASpellAimer::Start()
{
	bIsAiming = true;
	
	MoveToOwner();

	Start_Internal();
}

void ASpellAimer::Stop()
{
	bIsAiming = false;
	
	Stop_Internal();
}

UAimResultHolder* ASpellAimer::GetAimResult_Implementation() const
{
	return{};
}

void ASpellAimer::MoveToOwner()
{
	if (!OwnerSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerSocket is nullptr"));
		return;
	}
	
	SetActorLocation(OwnerSocket->GetComponentLocation());
}

