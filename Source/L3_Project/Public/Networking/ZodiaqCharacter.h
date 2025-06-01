#pragma once
#include "GameFramework/Character.h"
#include "Vitals/IAliveState.h"
#include "ZodiaqCharacter.generated.h"

class AZodiaqPlayerState;

UCLASS(BlueprintType, Blueprintable)
class AZodiaqCharacter : public ACharacter, public IAliveState
{
	GENERATED_BODY()

	bool bIsLoaded = false;
	
	virtual void BeginPlay() override;
	
	void InitSpells(APlayerController* PlayerController, AZodiaqPlayerState* ZodiaqPlayerState);

public:
	void LoadCharacterFromPlayerState(AZodiaqPlayerState* ZodiaqPlayerState);

	UFUNCTION(BlueprintImplementableEvent)
	void LoadCharacterFromPlayerState_BP(AZodiaqPlayerState* ZodiaqPlayerState);
};
