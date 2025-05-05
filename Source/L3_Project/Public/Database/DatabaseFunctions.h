#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "DatabaseFunctions.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FAuthSuccess, FString, PlayerID);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAuthFailed, FString, ErrorMessage);

/**
 * 
 */
UCLASS()
class L3_PROJECT_API UDatabaseFunctions : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	/*UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void AuthRequest(const FString& Email, const FString& Password, const FAuthSuccess& OnSuccess, const FAuthFailed& OnFailure);

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
	static FString LoadFirebaseApiKey();*/
};