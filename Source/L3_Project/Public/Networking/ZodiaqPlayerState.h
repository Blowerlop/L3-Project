#pragma once
#include "InstanceSettings.h"
#include "GameFramework/PlayerState.h"

#include "ZodiaqPlayerState.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AZodiaqPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	FClientData ClientData;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
