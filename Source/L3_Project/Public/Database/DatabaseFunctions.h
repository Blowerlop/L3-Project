#pragma once
 
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "DatabaseFunctions.generated.h"
 
DECLARE_DYNAMIC_DELEGATE_OneParam(FSuccess, FString, Data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFailed, FString, ErrorMessage);
 
/**
 * 
 */
UCLASS()
class L3_PROJECT_API UDatabaseFunctions : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void AuthRequest(const FString& Email, const FString& Password, const FSuccess& OnSuccess, const FFailed& OnFailure);

public:
	// Register User
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void RegisterRequest(const FString& UserName, const FString& Email, const FString& Password, const FSuccess& OnSuccess, const FFailed& OnFailure);

public:
	// Set Post Register User Data
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void SetPostRegisterData(const FString& UserName, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

public:
	// Bind UID to UserName
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void LinkUserIDAndName(const FString& UserName, const FString& UserId, const FSuccess& OnSuccess, const FFailed& OnFailure);

public:
	// Get User Data
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void GetData(const FString& Path, const FString& DataID, const FSuccess& OnSuccess, const FFailed& OnFailure);

public:
	// Set User Data
	static void SetData(const FString& Path, const TSharedPtr<FJsonObject> Data, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

private:
	// Get API Key
	static FString LoadFirebaseApiKey();
	
private:
	// Check User Availability
	static void CheckUserAvailability(const FString& Username, const TFunction<void(bool)>& Callback);

public:
	// Hash
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static FString HashString(const FString& target);
};