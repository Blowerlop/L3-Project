// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InstanceDatabase.generated.h"

class UInstanceDataAsset;
/**
 * 
 */
UCLASS(Blueprintable)
class L3_PROJECT_API UInstanceDatabase : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
public:
	UInstanceDataAsset* GetInstanceDataAsset(int32 AssetID) const;

	UPROPERTY(BlueprintReadOnly, Category = "Instance Database")
	TMap<int32, UInstanceDataAsset*> DataAssetMap;
};
