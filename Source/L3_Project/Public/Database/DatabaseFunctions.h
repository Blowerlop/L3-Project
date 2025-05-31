#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "DatabaseFunctions.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FSuccess, FString, Data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFailed, FString, ErrorMessage);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FGetCharacterCallback, bool, IsValid, FString, Response, APlayerController*, PlayerController);

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

    static void GetCharacterData(APlayerController* PlayerController, const FString& UserID, const FString& CharacterID, const FString& IdToken, const FGetCharacterCallback& Callback);
    
    // New Chara
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static FString CreateCharacter(const FString& UserName, const FString& IdToken, const FString& CharacterName, int WeaponID, int SelectedSpells, const FSuccess& OnSuccess, const FFailed& OnFailure);

    static void DeleteCharacter(const FString& UserID, const FString& CharacterID, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);
    
    // Get all chara
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static void GetAllCharacters(const FString& UID, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

    // Hash
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static FString HashString(const FString& target);

    UFUNCTION(BlueprintCallable, Category = "Firebase", BlueprintPure)
    static FString GetIdToken();
    
    UFUNCTION(BlueprintCallable, Category = "Firebase")
    static void Logout(UObject* WorldContext);

    // API KEY
    static FString LoadFirebaseApiKey();

    static bool LoadFirebaseAdminConfig(FString& OutAdminID, FString& OutAdminPassword);
    
private:
    // Check if the username is available
    static void CheckUserAvailability(const FString& Username, const TFunction<void(bool)>& Callback);

    // Check if the Chara name is available
    static void CheckCharacterNameAvailability(const FString& UserName, const FString& CharacterName, const FString& IdToken, const TFunction<void(bool)>& Callback);

    // Set Data to firebase
    static void SetData(const FString& Path, const TSharedPtr<FJsonObject> Data, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

public:
    static void DeleteData(const FString& Path, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);
    

    //Set Player data
    static void SetPlayerData(const FString& UserName, const FString& FieldName,
                       const FString& NewValue, const FString& IdToken, const FSuccess& OnSuccess,
                       const FFailed& OnFailure);
	
	//Set Player data (character specific)
    static void SetCharacterData_String(const FString& UserName, const FString& CharacterID, const FString& FieldName,
                       const FString& NewValue, const FString& IdToken, const FSuccess& OnSuccess,
                       const FFailed& OnFailure);
	
    //Set Player data (character specific)
    static void SetCharacterData_Int(const FString& UserName, const FString& CharacterID, const TArray<FString>& FieldName,
                       const TArray<int>& NewValue, const FString& IdToken, const FSuccess& OnSuccess,
                       const FFailed& OnFailure);
};