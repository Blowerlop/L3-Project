// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "ProjectCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class L3_PROJECT_API UProjectCheatManager : public UCheatManager
{
	GENERATED_BODY()

	UFUNCTION(exec, Category="Cheat Manager")
	void Project_AddHealth(const int Amount, int PlayerIndex = 0) const;

	UFUNCTION(exec, Category = "Cheat Manager")
	void Project_RemoveHealth(const int Amount, const int PlayerIndex = 0) const;

	void ModifyHealth(const int Amount, const int PlayerIndex) const;
};