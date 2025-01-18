// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InSceneManagersRefs.generated.h"

/**
 * 
 */
UCLASS()
class L3_PROJECT_API UInSceneManagersRefs : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<UClass*, AActor*> InSceneManagers;

public:
	UFUNCTION(BlueprintCallable)
	void RegisterManager(UClass* ManagerClass, AActor* ManagerInstance);

	UFUNCTION(BlueprintCallable)
	void UnregisterManager(UClass* ManagerClass);

	UFUNCTION(BlueprintCallable)
	AActor* GetManager(UClass* ManagerClass) const;

	template<typename T>
	T* GetManager() const
	{
		return Cast<T>(GetManager(T::StaticClass()));
	}
};
