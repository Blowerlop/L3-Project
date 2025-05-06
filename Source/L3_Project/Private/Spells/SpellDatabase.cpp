// Fill out your copyright notice in the Description page of Project Settings.

#include "Spells/SpellDatabase.h"

#include "Engine/AssetManager.h"
#include "Spells/SpellDataAsset.h"


void USpellDatabase::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Initializing Spell Database"));

	const FPrimaryAssetType DataType = TEXT("BP_SpellDataAsset_C");
	TArray<FAssetData> AssetList;
    
	UAssetManager::Get().GetPrimaryAssetDataList(DataType, AssetList);

	for (const FAssetData& AssetData : AssetList)
	{
		if (USpellDataAsset* LoadedAsset = Cast<USpellDataAsset>(AssetData.GetAsset()))
		{
			DataAssets.Add(LoadedAsset->AssetID, LoadedAsset);
		}
	}
}
