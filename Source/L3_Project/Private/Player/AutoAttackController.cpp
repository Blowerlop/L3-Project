// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AutoAttackController.h"

#include "InSceneManagersRefs.h"
#include "Spells/SpellManager.h"
#include "Spells/Results/VectorAimResultHolder.h"


// Sets default values for this component's properties
UAutoAttackController::UAutoAttackController()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UAutoAttackController::TriggerAutoAttack(UAimResultHolder* AimResultHolder)
{
	const auto ResultClass = AimResultHolder->GetClass();
	
	// Can't switch. Only works on ints
	if (ResultClass == UVectorAimResultHolder::StaticClass())
	{
		const auto VectorResult = Cast<UVectorAimResultHolder>(AimResultHolder);
		TriggerAutoAttackServerRpc_Vector(VectorResult->Vector);
	}
}

void UAutoAttackController::TriggerAutoAttackServerRpc_Vector_Implementation(const FVector Result)
{
	const auto Holder = NewObject<UVectorAimResultHolder>();
	Holder->Vector = Result;

	const auto InSceneManagers = GetWorld()->GetGameInstance()->GetSubsystem<UInSceneManagersRefs>();
	const auto SpellManager = InSceneManagers->GetManager<ASpellManager>();
	
	SpellManager->TryCastSpell(AutoAttackSpell, GetOwner(), Holder);
}

