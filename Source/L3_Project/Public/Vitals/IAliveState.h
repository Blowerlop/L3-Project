#pragma once
#include "IAliveState.generated.h"

UINTERFACE(BlueprintType, Blueprintable)
class UAliveState : public UInterface
{
	GENERATED_BODY()
};

class IAliveState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetIsDown(bool bIsDown);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetIsDead(bool bIsDead);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	bool GetIsAlive();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	bool GetIsDown();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	bool GetIsDead();
};
