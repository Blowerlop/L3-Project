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
	void AddHealth(const int Amount) const;

	UFUNCTION(exec, Category = "Cheat Manager")
	void RemoveHealth(const int Amount) const;
};
