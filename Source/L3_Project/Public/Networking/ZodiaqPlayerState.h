#pragma once
#include "InstanceSettings.h"
#include "GameFramework/PlayerState.h"

#include "ZodiaqPlayerState.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AZodiaqPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	bool bIsCharacterLoaded = false;
	
	UPROPERTY(BlueprintReadOnly)
	FClientData ClientData;

	void LoadCharacter(FSerializableCharacterData CharacterData);
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
