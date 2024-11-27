// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/InstanceDataAsset.h"


void UInstanceDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	AssetID = GetTypeHash(this->GetPathName());
}
