#include "InstancesDatabase.h"

#include "InstanceDataAsset.h"

TMap<FString, UInstanceDataAsset*> FInstancesDatabase::LoadedDatas{};

bool FInstancesDatabase::LoadInstanceData(const FString& AssetPath, UInstanceDataAsset*& OutData)
{
	if (LoadedDatas.Contains(AssetPath))
	{
		OutData = LoadedDatas[AssetPath];
		return true;
	}
	
	const FSoftObjectPath Path(AssetPath);

	if (const auto LoadedAsset = Cast<UInstanceDataAsset>(Path.TryLoad()))
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully loaded asset: %s"), *LoadedAsset->GetName());

		OutData = LoadedAsset;
		LoadedDatas.Add(AssetPath, LoadedAsset);
		return true;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Failed to load asset from path: %s"), *AssetPath);
	return false;
}
