// Fill out your copyright notice in the Description page of Project Settings.


#include "Cheats/ProjectCheatManager.h"

#include "GameFramework/Character.h"
#include "Vitals/VitalsContainer.h"

void UProjectCheatManager::AddHealth(const int Amount) const
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddHealth called with non-positive amount: %d"), Amount);
		return;
	}

	const auto PlayerController = GetPlayerController();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddHealth called but PlayerController is null!"));
		return;
	}

	const auto Character = PlayerController->GetCharacter();
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddHealth called but PlayerController is null!"));
		return;
	}

	const auto VitalsContainer = Character->GetComponentByClass<UVitalsContainer>();
	if (VitalsContainer == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddHealth called but VitalsContainer is null!"));
		return;
	}

	VitalsContainer->SrvAdd(EVitalType::Health, Amount);
}

void UProjectCheatManager::RemoveHealth(const int Amount) const
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddHealth called with non-positive amount: %d"), Amount);
		return;
	}

	const auto PlayerController = GetPlayerController();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddHealth called but PlayerController is null!"));
		return;
	}

	const auto Character = PlayerController->GetCharacter();
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddHealth called but PlayerController is null!"));
		return;
	}

	const auto VitalsContainer = Character->GetComponentByClass<UVitalsContainer>();
	if (VitalsContainer == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddHealth called but VitalsContainer is null!"));
		return;
	}

	VitalsContainer->SrvRemove(EVitalType::Health, Amount);
}
