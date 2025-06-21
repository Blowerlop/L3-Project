// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Networking/BaseGameSession.h"
#include "LobbyGameSession.generated.h"

/**
 * 
 */
UCLASS()
class L3_PROJECT_API ALobbyGameSession : public ABaseGameSession
{
	GENERATED_BODY()
	
	virtual void UnregisterPlayer(const APlayerController* ExitingPlayer) override;
};
