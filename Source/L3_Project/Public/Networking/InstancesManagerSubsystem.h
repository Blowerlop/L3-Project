// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameInstance.h"
#include "InstanceSettings.h"
#include "SessionsManagerSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InstancesManagerSubsystem.generated.h"

struct FInstanceSettings;
class UInstanceDataAsset;
struct FBlueprintSessionSearchResult;

/**
 * 
 */
UCLASS()
class L3_PROJECT_API UInstancesManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static int InstanceIDCounter;
	static bool IsInstanceBeingDestroyed;

	static FServerInstanceSettings CurrentInstanceSettings;
	
	UFUNCTION(BlueprintCallable, Category = "Online Session")
	void StartNewInstance(FServerInstanceSettings Settings);
	
	UFUNCTION(BlueprintCallable, Category = "Online Session")
	void StopInstance();
	
	UFUNCTION(BlueprintCallable, Category = "Online Session")
	void JoinInstance(FName SessionName, FBlueprintSessionSearchResult SessionData);
	
	UFUNCTION(BlueprintCallable, Category = "Online Session")
	void ReturnToLobby();

	static int GetNextInstanceID()
	{
		return InstanceIDCounter++;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Online Session")
	static FServerInstanceSettings& GetCurrentInstanceSettings()
	{
		return CurrentInstanceSettings;
	}

	UFUNCTION(BlueprintCallable, Category = "Networking")
	static FName GetOnlineSubsystemName()
	{
		return "EOS";

		// Regular Unreal doesn't have UWorld::HostingType
		//return FName(UWorld::HostingType == EHostingType::EOS ? "EOS" : "NULL");
	}

private:
	void StartListenServer(const FString& InstanceMapPath) const;

	bool TryGetBaseGameInstance(UBaseGameInstance*& Out) const
	{
		Out = Cast<UBaseGameInstance>(GetGameInstance());
		return IsValid(Out);
	}

	bool TryGetSessionsManager(USessionsManagerSubsystem*& Out) const
	{
		Out = GetGameInstance()->GetSubsystem<USessionsManagerSubsystem>();
		return IsValid(Out);
	}

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	static void SetHostingType(const FString& Args);
};
