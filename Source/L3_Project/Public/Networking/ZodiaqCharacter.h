#pragma once
#include "GameFramework/Character.h"
#include "Vitals/IAliveState.h"
#include "ZodiaqCharacter.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AZodiaqCharacter : public ACharacter, public IAliveState
{
	GENERATED_BODY()
};
