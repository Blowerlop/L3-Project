// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AutoAttackController.h"

#include "InSceneManagersRefs.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Spells/SpellDataAsset.h"
#include "Spells/SpellManager.h"
#include "Spells/Results/VectorAimResultHolder.h"


// Sets default values for this component's properties
UAutoAttackController::UAutoAttackController()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UAutoAttackController::SendAttackResponse(UAimResultHolder* Result)
{
	if (!UKismetSystemLibrary::IsServer(this)) return;
	
	AttackState->AimResult = Result;

	AttackResponseMultiCastRpc();
}

void UAutoAttackController::SrvOnAnimationAttackSpellNotify()
{
	if (!UKismetSystemLibrary::IsServer(this)) return;
	
	const auto InSceneManagers = GetWorld()->GetGameInstance()->GetSubsystem<UInSceneManagersRefs>();
	const auto SpellManager = InSceneManagers->GetManager<ASpellManager>();
	
	SpellManager->TryCastSpell(AttackState->AttackSpell, GetOwner(), AttackState->AimResult);
}

void UAutoAttackController::OnAnimationEndedNotify()
{
	AttackState->IsAttacking = false;
	AttackState->AttackSpell = nullptr;
	AttackState->AnimationCompletion = 0.0f;
}

void UAutoAttackController::UpdateAnimationCompletion(float Value)
{
	AttackState->AnimationCompletion = Value;
}

bool UAutoAttackController::IsAttacking() const
{
	return AttackState->IsAttacking;
}

void UAutoAttackController::AttackResponseMultiCastRpc_Implementation()
{
	AttackState->IsAttacking = true;
	AttackState->AttackSpell = AutoAttackSpell;

	OnAttackStart.Broadcast(AttackState->AttackSpell);
}

void UAutoAttackController::RequestAutoAttack(UAimResultHolder* AimResultHolder)
{
	const auto ResultClass = AimResultHolder->GetClass();
	
	// Can't switch. Only works on ints
	if (ResultClass == UVectorAimResultHolder::StaticClass())
	{
		const auto VectorResult = Cast<UVectorAimResultHolder>(AimResultHolder);
		RequestAutoAttackServerRpc_Vector(VectorResult->Vector);
	}
}

void UAutoAttackController::BeginPlay()
{
	Super::BeginPlay();

	AttackState = NewObject<UControllerAttackState>();
}

void UAutoAttackController::RequestAutoAttackServerRpc_Vector_Implementation(const FVector Result)
{
	const auto Holder = NewObject<UVectorAimResultHolder>();
	Holder->Vector = Result;

	const auto InSceneManagers = GetWorld()->GetGameInstance()->GetSubsystem<UInSceneManagersRefs>();
	const auto SpellManager = InSceneManagers->GetManager<ASpellManager>();
	
	SpellManager->RequestAttack(AutoAttackSpell, this, Holder);
}


