// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "BaseGameSession.generated.h"

/**
 * 
 */
UCLASS()
class L3_PROJECT_API ABaseGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override;
	
private:
	virtual void NotifyLogout(const APlayerController* PC) override;
	
	void StartSession();
	virtual void HandleStartSessionCompleted(FName EosSessionName, bool bWasSuccessful);

	void EndSession();
	virtual void HandleEndSessionCompleted(FName EosSessionName, bool bWasSuccessful);

	FDelegateHandle RegisterPlayerDelegateHandle;
	FDelegateHandle UnregisterPlayerDelegateHandle;

	FDelegateHandle StartSessionDelegateHandle;
	FDelegateHandle EndSessionDelegateHandle;

protected:
	int RegisteredPlayerCount;

	virtual void UnregisterPlayer(const APlayerController* ExitingPlayer) override;
	
	virtual void HandleRegisterPlayerCompleted(FName EosSessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccessful);
	virtual void HandleUnregisterPlayerCompleted(FName EosSessionName, const TArray<FUniqueNetIdRef>& PlayerIds, bool bWasSuccessful);
};
