// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/InstanceDataAsset.h"

#if WITH_EDITOR
void UInstanceDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	AssetID = GetTypeHash(this->GetPathName());
}
#endif
