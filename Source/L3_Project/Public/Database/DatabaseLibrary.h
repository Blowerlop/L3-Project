#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DatabaseLibrary.generated.h"

UCLASS()
class L3_PROJECT_API UDatabaseLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Auth User
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void FirebaseAuthRequest(const FString& Email, const FString& Password);
};