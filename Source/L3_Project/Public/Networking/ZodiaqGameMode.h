// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Database/DatabaseFunctions.h"
#include "GameFramework/GameModeBase.h"
#include "ZodiaqGameMode.generated.h"

/**
 * 
 */

class AZodiaqCharacter;
class AZodiaqPlayerState;


UCLASS()
class L3_PROJECT_API AZodiaqGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	void OnPlayerStateEndPlay(const AZodiaqPlayerState* PlayerState) const;
	
	static void DisconnectPlayer(APlayerController* PlayerController);
	
protected:
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	UFUNCTION()
	void GetCharacterCallback(bool CharacterValid, const FString& Response, APlayerController* PlayerController);

private:
	FGetCharacterCallback GetCharacterCallbackDelegate;
};
