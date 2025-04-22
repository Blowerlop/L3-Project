#pragma once

#include "InstanceSettings.generated.h"

class UInstanceDataAsset;

USTRUCT(BlueprintType)
struct FInstanceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instance Settings")
	UInstanceDataAsset* DataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instance Settings")
	int32 SessionID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instance Settings")
	int32 PlayerCount;
};
