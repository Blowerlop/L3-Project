// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SessionsManagerSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FBlueprintSessionSearchResult
{
	GENERATED_USTRUCT_BODY()
	
	FOnlineSessionSearchResult OnlineResult;

	static FBlueprintSessionSearchResult GetFromCPP(const FOnlineSessionSearchResult& OnlineResult)
	{
		FBlueprintSessionSearchResult BlueprintResult;
		BlueprintResult.OnlineResult = OnlineResult;
		return BlueprintResult;
	}
};

/**
 * 
 */
UCLASS()
class L3_PROJECT_API USessionsManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FFindSessionsDelegate, bool, bWasSuccessful, const FBlueprintSessionSearchResult&, Search);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FCreateSessionDelegate, FName, SessionName, bool, bWasSuccessful);
	
	DECLARE_DELEGATE_OneParam(FDestroySessionDelegate, bool);
	DECLARE_DELEGATE_TwoParams(FFindSessionsDelegateCPP, bool bWasSuccessful, const FBlueprintSessionSearchResult& Search);
	
public:
	static bool IsSessionHost;
	static bool HasRunningSession;
	
	static FName RunningSessionName;
	
	UFUNCTION(BlueprintCallable, Category = "Custom Online Session", meta = (AutoCreateRefTerm = "Delegate"))
	void CreateSession(FName SessionName, FCreateSessionDelegate Delegate, FName KeyName = "KeyName",
		FString KeyValue = "KeyValue", bool bDedicatedServer = true);

	UFUNCTION(BlueprintCallable, Category = "Custom Online Session")
	void DestroySession();

	typedef std::function<void(const bool)> DSWCFunc;
	void DestroySessionWithCallback(const DSWCFunc& Func);
	
	UFUNCTION(BlueprintCallable, Category = "Custom Online Session", meta = (AutoCreateRefTerm = "Delegate"))
	void FindSessions(FName SearchKey, FString SearchValue, FFindSessionsDelegate Delegate);

	typedef std::function<void(bool bWasSuccessful, const FBlueprintSessionSearchResult& Search)> FindSessionFunc;
	void FindSessions(FName SearchKey, FString SearchValue, const FindSessionFunc& Func);

	void FindSessions(FName SearchKey, FString SearchValue);
	
	UFUNCTION(BlueprintCallable, Category = "Custom Online Session")
	void JoinSession(FName SessionName, FBlueprintSessionSearchResult SessionData);

	UFUNCTION(BlueprintCallable, Category = "Custom Online Session")
	void RegisterSelf();
	
private:
	FDelegateHandle CreateSessionDelegateHandle;
	FCreateSessionDelegate BP_CreateSessionDelegate;
	
	FDelegateHandle DestroySessionDelegateHandle;
	FDestroySessionDelegate DestroySessionDelegate;

	FDelegateHandle FindSessionsDelegateHandle;
	FFindSessionsDelegate BP_FindSessionsDelegate;
	FFindSessionsDelegateCPP CPP_FindSessionsDelegate;
	
	FDelegateHandle JoinSessionDelegateHandle;
	
	const int MaxNumberOfPlayersInSession = 5;

	void HandleCreateSessionCompleted(FName EosSessionName, bool bWasSuccessful);
	
	void HandleDestroySessionCompleted(FName EosSessionName, bool bWasSuccessful);
	
	void HandleFindSessionsCompleted(bool bWasSuccessful, TSharedRef<FOnlineSessionSearch> Search);
	void HandleJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
