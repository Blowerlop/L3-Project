// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InstanceDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class L3_PROJECT_API UInstanceDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AssetID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InstanceName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapPath = "/Game/_Project/000-Game/Maps/InstanceMap";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InstanceImage;

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

};
