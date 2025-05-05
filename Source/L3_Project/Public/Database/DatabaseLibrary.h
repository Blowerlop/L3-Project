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
	static void AuthRequest(const FString& Email, const FString& Password);

public:
	// Register User
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void RegisterRequest(const FString& UserName, const FString& Email, const FString& Password);

public:
	// Get User Data
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void GetData(const FString& UserID, const FString& DataID);

private:
	// Get API Key
	static FString LoadFirebaseApiKey();
};