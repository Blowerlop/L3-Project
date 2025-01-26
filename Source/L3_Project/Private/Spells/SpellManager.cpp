// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/SpellManager.h"

#include "InSceneManagersRefs.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/AutoAttackController.h"
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

void ASpellManager::RequestSpellCastFromController(const int SpellIndex, USpellController* SpellController, UAimResultHolder* Result) const
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("TryCastSpellFromController can only be called on the server!"));
		return;
	}

	if (SpellController->IsCasting())
	{
		UE_LOG(LogTemp, Error, TEXT("Can't cast spell while casting!"));
		return;
	}
	
	SpellController->SendSpellCastResponse(SpellIndex, Result);
	
	/*TryCastSpell(SpellData, SpellController->GetOwner(), Result);

	SpellController->StartCooldown(SpellIndex);*/
	SpellController->StartGlobalCooldown();
}

void ASpellManager::RequestAttack(USpellDataAsset* AttackSpell, UAutoAttackController* AttackController,UAimResultHolder* Result) const
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("TryCastAttackSpell can only be called on the server!"));
		return;
	}

	AttackController->SendAttackResponse(Result);
}

void ASpellManager::TryCastSpell(USpellDataAsset* SpellData, AActor* Caster, UAimResultHolder* Result) const
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("TryCastSpell can only be called on the server!"));
		return;
	}

	if (!IsValid(SpellData))
	{
		UE_LOG(LogTemp, Error, TEXT("SpellData is not valid!"));
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

