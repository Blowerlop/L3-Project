#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "DatabaseFunctions.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FSuccess, FString, Data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFailed, FString, ErrorMessage);

UCLASS()
class L3_PROJECT_API UDatabaseFunctions : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    // AUTH
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static void AuthRequest(const FString& Email, const FString& Password, const FSuccess& OnSuccess, const FFailed& OnFailure);

    // REGISTER
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static void RegisterRequest(const FString& UserName, const FString& Email, const FString& Password, const FSuccess& OnSuccess, const FFailed& OnFailure);

    // POST DATA
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static void SetPostRegisterData(const FString& UserName, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

    // LINK TO DB
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static void LinkUserIDAndName(const FString& UserName, const FString& UserId, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

    // Get Data
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static void GetData(const FString& Path, const FString& DataID, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

    // New Chara
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static void CreateCharacter(const FString& UserName, const FString& IdToken, const FString& CharacterName, int WeaponID, int SelectedSpells, const FSuccess& OnSuccess, const FFailed& OnFailure);

    // Get all chara
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static void GetAllCharacters(const FString& UID, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

    // Hash
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static FString HashString(const FString& target);

    UFUNCTION(BlueprintCallable, Category = "Firebase", BlueprintPure)
    static FString GetIdToken();
    
private:
    // API KEY
    static FString LoadFirebaseApiKey();

    // Check if the username is available
    static void CheckUserAvailability(const FString& Username, const TFunction<void(bool)>& Callback);

    // Check if the Chara name is available
    static void CheckCharacterNameAvailability(const FString& UserName, const FString& CharacterName, const FString& IdToken, const TFunction<void(bool)>& Callback);

    // Set Data to firebase
    static void SetData(const FString& Path, const TSharedPtr<FJsonObject> Data, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);
};