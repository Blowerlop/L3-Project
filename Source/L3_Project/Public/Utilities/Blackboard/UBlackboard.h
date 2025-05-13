#pragma once

#include "CoreMinimal.h"
#include "UBlackboard.generated.h"

class UBlackboardComponent;
/**
 * 
 */
UCLASS()
class L3_PROJECT_API UBlackboard : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Blackboard")
	static TArray<FName> GetAllKeys(const UBlackboardComponent* blackboard);
};
