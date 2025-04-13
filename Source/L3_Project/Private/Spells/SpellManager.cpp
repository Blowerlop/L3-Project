// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/SpellManager.h"

#include "InSceneManagersRefs.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
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

void ASpellManager::RequestSpellCastFromController(const int SpellIndex, USpellController* SpellController,
	UAimResultHolder* Result, double ClientTime) const
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("TryCastSpellFromController can only be called on the server!"));
		return;
	}

	const auto CastState = SpellController->CastState;
	const auto Spell = CastState->Spell;
	
	if (SpellIndex == CastState->SpellIndex && IsInComboWindow(Spell, ClientTime, CastState->AnimationStartTime, CastState->AnimationEndTime))
	{
		SpellController->SendSpellCastResponse(SpellIndex, CastState->AimResult, Spell->NextComboSpell);
	}
	else
	{
		if (SpellController->IsCasting()) return;

		// No combo
		SpellController->SendSpellCastResponse(SpellIndex, Result);
	}
	
	SpellController->StartGlobalCooldown();
}

void ASpellManager::RequestAttack(UAutoAttackController* AttackController, UAimResultHolder* Result, const double ClientTime) const
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("TryCastAttackSpell can only be called on the server!"));
		return;
	}

	const auto AttackState = AttackController->AttackState;
	const auto AttackSpell = AttackState->AttackSpell;
	
	if (IsInComboWindow(AttackSpell, ClientTime, AttackState->AnimationStartTime, AttackState->AnimationEndTime))
	{
		AttackController->SendAttackResponse(AttackState->AimResult, AttackSpell->NextComboSpell);
	}
	else
	{
		if (AttackController->IsAttacking()) return;

		// No combo
		AttackController->SendAttackResponse(Result);
	}
}

ASpell* ASpellManager::TryCastSpell(USpellDataAsset* SpellData, AActor* Caster, UAimResultHolder* Result)
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("TryCastSpell can only be called on the server!"));
		return nullptr;
	}

	if (!IsValid(SpellData))
	{
		UE_LOG(LogTemp, Error, TEXT("SpellData is not valid!"));
		return nullptr;
	}

	if (!IsValid(Caster))
	{
		UE_LOG(LogTemp, Error, TEXT("Caster is not valid"));
		return nullptr;
	}

	const auto SpellClass = SpellData->Spell;
	const auto Location = Caster->GetActorLocation();

	const FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, Location);
	AActor* MySpawningActor = Cast<AActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, SpellClass, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

	if (const auto SpellInstance = Cast<ASpell>(MySpawningActor))
	{
		SpellInstance->Init(SpellData, Caster, Result);
		
		SpellInstance->Owner = Caster;
		SpellInstance->SetInstigator(Cast<APawn>(Caster));
		
		UGameplayStatics::FinishSpawningActor(MySpawningActor, SpawnTransform);

		return SpellInstance;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to spawn spell actor!"));
	return nullptr;
}

bool ASpellManager::IsInComboWindow(const USpellDataAsset* Spell, const double ClientTime, const double StartTime, const double EndTime)
{
	if (!IsValid(Spell) || !Spell->bHasCombo) return false;
	if (ClientTime >= EndTime || ClientTime <= StartTime) return false;
	
	const auto AnimationLength = Spell->AnimationMontage->GetSectionLength(Spell->ComboIndex);

	const auto DiffWithClient = ClientTime - StartTime;

	const auto Completion = DiffWithClient / AnimationLength;

	if (Completion < Spell->ComboWindow.X || Completion > Spell->ComboWindow.Y)
		return false;

	return true;
}

