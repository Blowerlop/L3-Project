// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

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

/**
 * 
 */
UCLASS()
class L3_PROJECT_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	DECLARE_DELEGATE(FTransitionDelegate);
	
public:
	UFUNCTION(BlueprintCallable, Category = "Custom Online Session")
	void Login(bool bUseDevTool, FString AuthToolId = "") const;

	UFUNCTION(BlueprintCallable, Category = "Custom Online Session")
	void Logout() const;

	UFUNCTION(BlueprintCallable, Category = "Online Session")
	bool IsLoggedIn() const;

	typedef std::function<void()> TransitionFunc;
	void StartTransition(ENetTransitionType TransitionType);
	void StartTransition(ENetTransitionType TransitionType, const TransitionFunc& Func);
	
	UFUNCTION(BlueprintCallable, Category = "Online Session")
	void OnTransitionEntered();
	
private:
	FTransitionDelegate TransitionDelegate;

	IConsoleCommand* LoginWithDevAuthToolCommand;
	
	virtual void Init() override;
	virtual void Shutdown() override;
	
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String);

	UFUNCTION(Exec)
	void DebugLoginWithDevAuthTool(const FString& Args) const;

	UFUNCTION(Exec)
	static void SetHostingType(const FString& Args);
};
