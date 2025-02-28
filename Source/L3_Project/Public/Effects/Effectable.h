#pragma once
#include "Effectable.generated.h"

class UEffectInstance;

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class L3_PROJECT_API UEffectable : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void AddEffect();

	UFUNCTION(BlueprintCallable)
	void RemoveEffect(UEffectInstance* Effect);
};
