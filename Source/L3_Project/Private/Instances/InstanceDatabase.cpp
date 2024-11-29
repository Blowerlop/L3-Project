// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/InstanceDatabase.h"

#include "Engine/AssetManager.h"
#include "Instances/InstanceDataAsset.h"


void UInstanceDatabase::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Initializing Instance Database"));

	const FPrimaryAssetType DataType = TEXT("InstanceDataAsset");
	TArray<FAssetData> AssetList;
    
	UAssetManager::Get().GetPrimaryAssetDataList(DataType, AssetList);

	UE_LOG(LogTemp, Log, TEXT("IInstance Database: found %d"), AssetList.Num());
	
	for (const FAssetData& AssetData : AssetList)
	{
		if (UInstanceDataAsset* LoadedAsset = Cast<UInstanceDataAsset>(AssetData.GetAsset()))
		{
			DataAssetMap.Add(LoadedAsset->AssetID, LoadedAsset);
		}
	}
}

UInstanceDataAsset* UInstanceDatabase::GetInstanceDataAsset(const int32 AssetID) const
{
	if (!DataAssetMap.Contains(AssetID))
	{
		return nullptr;
	}
	
	return DataAssetMap[AssetID];
}
