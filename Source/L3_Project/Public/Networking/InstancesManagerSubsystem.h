// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameInstance.h"
#include "SessionsManagerSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InstancesManagerSubsystem.generated.h"

class UInstanceDataAsset;
struct FBlueprintSessionSearchResult;

UENUM(BlueprintType)
enum class EHostingType : uint8
{
	EOS,
	LANBroadcast,
	IP
};

/**
 * 
 */
UCLASS()
class L3_PROJECT_API UInstancesManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static int InstanceIDCounter;
	static int InstanceSessionID;
	static bool IsInstanceBeingDestroyed;

	static EHostingType HostingType;
	
	UFUNCTION(BlueprintCallable, Category = "Online Session")
	void StartNewInstance(int SessionID, UInstanceDataAsset* Data);
	
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
	static int GetCurrentInstanceID()
	{
		return InstanceSessionID;
	}

	UFUNCTION(BlueprintCallable, Category = "Networking")
	static FName GetOnlineSubsystemName()
	{
		return FName(HostingType == EHostingType::EOS ? "EOS" : "NULL");
	}

private:
	const FName LobbyOnlineSubsystem = "EOS";
	
	void StartListenServer(const int SessionID, const FString& InstanceMapPath) const;

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
	static void SetHostingType(const FString& Args);
};
