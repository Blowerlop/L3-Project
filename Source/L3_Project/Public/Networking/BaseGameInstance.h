// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "InstanceSettings.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

class AZodiaqCharacter;
class UInstanceDataAsset;
class UInstancesManagerSubsystem;

UENUM(BlueprintType)
enum class EDisconnectType : uint8
{
	NetworkFailure UMETA(DisplayName="Network Failure"),
	TransitionMap UMETA(DisplayName="Transition Map"), 
};

UENUM(BlueprintType)
enum class ENetTransitionType : uint8
{
	LobbyToInstance UMETA(DisplayName="Lobby To Instance"),
	InstanceToLobby UMETA(DisplayName="Instance To Lobby"), 
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSpawned, AZodiaqCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDestroyed, AZodiaqCharacter*, Character);

/**
 * 
 */
UCLASS()
class L3_PROJECT_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	DECLARE_DELEGATE(FTransitionDelegate);
	
public:
	static const FString UUIDConnectOptionsKey;
	static const FString UserNameConnectOptionsKey;
	static const FString CharacterUUIDConnectOptionsKey;
	
	static FString FirebaseIdToken;

	UPROPERTY(BlueprintReadWrite)
	FString LastNetworkFailure{};
	
	UPROPERTY(BlueprintReadWrite)
	FClientData SelfClientData;

	UFUNCTION(BlueprintCallable, Category = "Custom Online Session")
	void Login(FString StandardWayType, bool bUseDevTool, FString AuthToolId = "");

	UFUNCTION(BlueprintCallable, Category = "Custom Online Session")
	void Logout() const;

	UFUNCTION(BlueprintCallable, Category = "Online Session")
	bool IsLoggedIn() const;

	typedef std::function<void()> TransitionFunc;
	void StartTransition(ENetTransitionType TransitionType);
	void StartTransition(ENetTransitionType TransitionType, const TransitionFunc& Func);
	
	UFUNCTION(BlueprintCallable, Category = "Online Session")
	void OnTransitionEntered();

	void OnFirebaseLogout();

	UPROPERTY(BlueprintAssignable)
	FOnCharacterSpawned OnCharacterLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterDestroyed OnCharacterDestroyed;

	void NotifyCharacterLoaded(AZodiaqCharacter* Character);
	void NotifyCharacterDestroyed(AZodiaqCharacter* Character);
	
private:
	FDelegateHandle LoginDelegateHandle;
	
	FTransitionDelegate TransitionDelegate;

	IConsoleCommand* LoginWithDevAuthToolCommand;
	
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION()
	void OnServerAuthSuccess(const FString& Message);
	UFUNCTION()
	void OnServerAuthFailure(const FString& ErrorMessage);
	
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String);

	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	
	UFUNCTION(Exec)
	void DebugLoginWithDevAuthTool(const FString& Args);

	UFUNCTION(Exec)
	static void SetHostingType(const FString& Args);
	
	UFUNCTION(Exec)
	static void PrintServerInfos();
};
