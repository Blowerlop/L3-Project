// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/SpellManager.h"

#include "InSceneManagersRefs.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Spells/Spell.h"
#include "Spells/SpellController.h"
#include "Spells/SpellDataAsset.h"

FSrvOnSpellCasted ASpellManager::SrvOnSpellCasted{};

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

void ASpellManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	const auto GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance) return;
	
	const auto InSceneManagers = GameInstance->GetSubsystem<UInSceneManagersRefs>();
	if (!InSceneManagers) return;
	
	InSceneManagers->UnregisterManager(StaticClass());
}

void ASpellManager::RequestSpellCastFromController(const int SpellIndex, USpellController* SpellController,
                                                   UAimResultHolder* Result) const
{
	if (!UKismetSystemLibrary::IsServer(this))
	{
		UE_LOG(LogTemp, Error, TEXT("TryCastSpellFromController can only be called on the server!"));
		return;
	}

	// Remove the latency between input and server receiving the input
	// + Remove latency between server animation start and client animation start
	const auto CompensatedTime = GetWorld()->GetTimeSeconds() - GetRttForControllerInSeconds(SpellController);

	switch(GetSpellRequestValidity(SpellIndex, SpellController, CompensatedTime))
	{
		case Invalid:
			SpellController->InvalidSpellCastResponseOwnerRpc();
			return;
		case Normal:
			SpellController->SendSpellCastResponse(SpellIndex, Result);
			break;
		case Combo:
			{
				const auto CastState = SpellController->CastState;
				const auto Spell = CastState->Spell;
			
				SpellController->SendSpellCastResponse(SpellIndex, CastState->AimResult, Spell->NextComboSpell);
			}
			break;
	}
	
	SpellController->StartGlobalCooldown();
}

ESpellRequestValidity ASpellManager::GetSpellRequestValidity(int SpellIndex, USpellController* SpellController,
	const double Time)
{
	const auto CastState = SpellController->CastState;
	const auto Spell = CastState->Spell;
	
	if (SpellIndex == CastState->SpellIndex && IsInComboWindow(Spell, Time, CastState->AnimationStartTime, CastState->AnimationEndTime))
	{
		return Combo;
	}
	
	if (SpellController->IsCasting()) return Invalid;

	return Normal;
}

ASpell* ASpellManager::TryCastSpell(USpellDataAsset* SpellData, AActor* Caster, UAimResultHolder* Result, float Duration)
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
		SpellInstance->Init(SpellData, Caster, Result, Duration);
		
		SpellInstance->Owner = Caster;
		SpellInstance->SetInstigator(Cast<APawn>(Caster));
		
		UGameplayStatics::FinishSpawningActor(MySpawningActor, SpawnTransform);

		SrvOnSpellCasted.Broadcast(SpellData, Caster);
		return SpellInstance;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to spawn spell actor!"));
	return nullptr;
}

// Checks if the given time is in the window based on animation duration and combo window.
// Does not handle lag compensation
bool ASpellManager::IsInComboWindow(const USpellDataAsset* Spell, const double ClientTime, const double StartTime, const double EndTime)
{
	if (!IsValid(Spell) || !Spell->bHasCombo) return false;
	if (ClientTime >= EndTime || ClientTime <= StartTime) return false;
	
	const auto AnimationLength = Spell->GetMontageSectionLength(Spell->ComboIndex);

	const auto DiffWithClient = ClientTime - StartTime;

	const auto Completion = DiffWithClient / AnimationLength;

	if (Completion < Spell->ComboWindow.X || Completion > Spell->ComboWindow.Y)
		return false;

	return true;
}

double ASpellManager::GetRttForControllerInSeconds(const USpellController* SpellController)
{
	const auto Owner = SpellController->GetOwner()->GetOwner();
	if (!IsValid(Owner)) return 0.0;

	const auto PlayerController = Cast<APlayerController>(Owner);
	if (!IsValid(PlayerController)) return 0.0;

	const auto PlayerState = PlayerController->PlayerState;
	if (!IsValid(PlayerState)) return 0.0;

	return static_cast<double>(PlayerState->ExactPing) / 1000.0;
}

