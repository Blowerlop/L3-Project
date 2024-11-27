#pragma once

class UInstanceDataAsset;

class FInstancesDatabase
{
public:
	static bool LoadInstanceData(const FString& AssetPath, UInstanceDataAsset*& OutData);

private:
	static TMap<FString, UInstanceDataAsset*> LoadedDatas;
};
