// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/SpellManager.h"

#include "InSceneManagersRefs.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Spells/Spell.h"
#include "Spells/SpellController.h"
#include "Spells/SpellDataAsset.h"

ASpellManager::ASpellManager()
{
	// Not needed
	PrimaryActorTick.bCanEverTick = false; 
}

void ASpellManager::BeginPlay()
{
	Super::BeginPlay();

	const auto InSceneManagers = GetWorld()->GetGameInstance()->GetSubsystem<UInSceneManagersRefs>();

	InSceneManagers->RegisterManager(StaticClass(), this);
}

void ASpellManager::Destroyed()
{
	Super::Destroyed();

	const auto GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance) return;
	const auto InSceneManagers = GameInstance->GetSubsystem<UInSceneManagersRefs>();
	if (!InSceneManagers) return;
	
	InSceneManagers->UnregisterManager(StaticClass());
}

void ASpellManager::TryCastSpellFromController(const int SpellIndex, AActor* Caster, UAimResultHolder* Result) const
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("TryCastSpellFromController can only be called on the server!"));
		return;
	}
	
	const auto SpellController = Caster->GetComponentByClass<USpellController>();

	if (!SpellController)
	{
		UE_LOG(LogTemp, Error, TEXT("Caster %s has no SpellController component"), *Caster->GetName());
		return;
	}

	const auto SpellData = SpellController->GetSpellData(SpellIndex);
	TryCastSpell(SpellData, Caster, Result);

	SpellController->StartCooldown(SpellIndex);
	SpellController->StartGlobalCooldown();
}

void ASpellManager::TryCastSpell(USpellDataAsset* SpellData, AActor* Caster, UAimResultHolder* Result) const
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("TryCastSpell can only be called on the server!"));
		return;
	}
	
	const auto SpellClass = SpellData->Spell;
	const auto Location = Caster->GetActorLocation();

	FActorSpawnParameters SpawnParams{};
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = Caster;
	// Is a pawn or nullptr
	SpawnParams.Instigator = Cast<APawn>(Caster);

	const auto SpellInstance = GetWorld()->SpawnActor<ASpell>(SpellClass, Location, FRotator::ZeroRotator, SpawnParams);
	SpellInstance->Init(SpellData, Caster, Result);
}

