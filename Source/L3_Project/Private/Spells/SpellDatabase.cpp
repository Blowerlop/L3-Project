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

TArray<USpellDataAsset*> USpellDatabase::GroupByConditionSpell(TArray<USpellDataAsset*> InSpells)
{
	Algo::Sort(InSpells, [](const USpellDataAsset* A, const USpellDataAsset* B)
	{
		const bool AIsNull = (A->WeaponSpellCondition == nullptr);
		const bool BIsNull = (B->WeaponSpellCondition == nullptr);

		if (AIsNull && BIsNull)
		{
			return A->AssetID < B->AssetID;
		}

		if (AIsNull)
		{
			return true;
		}

		if (BIsNull)
		{
			return false;
		}

		return A->WeaponSpellCondition->AssetID < B->WeaponSpellCondition->AssetID;
	});
	
	return InSpells;
}
