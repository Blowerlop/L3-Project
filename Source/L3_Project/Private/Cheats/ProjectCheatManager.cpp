// Fill out your copyright notice in the Description page of Project Settings.


#include "Cheats/ProjectCheatManager.h"

#include "GameFramework/PlayerState.h"
#include "Vitals/VitalsContainer.h"
#include "Kismet/GameplayStatics.h"

void UProjectCheatManager::Project_AddHealth(const int Amount, const int PlayerIndex) const
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Non-positive amount is not accepted: %d"), Amount);
		return;
	}

	ModifyHealth(Amount, PlayerIndex);
}

void UProjectCheatManager::Project_RemoveHealth(const int Amount, const int PlayerIndex) const
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Non-positive amount is not accepted: %d"), Amount);
		return;
	}

	ModifyHealth(-Amount, PlayerIndex);
}

void UProjectCheatManager::ModifyHealth(const int Amount, const int PlayerIndex) const
{
	const auto PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), PlayerIndex);
	if (PlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState is null"));
		return;
	}

	const auto Pawn = PlayerState->GetPawn();
	if (Pawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn is null"));
		return;
	}

	const auto VitalsContainer = Pawn->GetComponentByClass<UVitalsContainer>();
	if (VitalsContainer == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("VitalsContainer is null"));
		return;
	}

	if (Amount > 0)
	{
		VitalsContainer->SrvAdd(EVitalType::Health, Amount);
	}
	else
	{
		VitalsContainer->SrvRemove(EVitalType::Health, -Amount);
	}
}
