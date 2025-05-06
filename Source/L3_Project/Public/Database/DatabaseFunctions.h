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

public:
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void AuthRequest(const FString& Email, const FString& Password, const FAuthSuccess& OnSuccess, const FAuthFailed& OnFailure);

public:
	// Register User
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void RegisterRequest(const FString& UserName, const FString& Email, const FString& Password, const FAuthSuccess& OnSuccess, const FAuthFailed& OnFailure);

public:
	// Set Post Register User Data
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void SetPostRegisterData(const FString& UserName, const FString& IdToken, const FAuthSuccess& OnSuccess, const FAuthFailed& OnFailure);

public:
	// Get User Data
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void GetData(const FString& UserID, const FString& DataID);

public:
	// Set User Data
	static void SetData(const FString& Path, const TSharedPtr<FJsonObject> Data, const FString& IdToken, const FAuthSuccess& OnSuccess, const FAuthFailed& OnFailure);

private:
	// Get API Key
	static FString LoadFirebaseApiKey();
	
private:
	// Check User Availability
	static void CheckUserAvailability(const FString& Username, const TFunction<void(bool)>& Callback);
};