#pragma once
#include "CharacterData.h"
#include "Database/DatabaseFunctions.h"
#include "CharacterManagerSubsystem.generated.h"

class FSuccess;
DECLARE_DYNAMIC_DELEGATE_OneParam(FSaveCharacterDelegate, FString, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FCreateCharacterDelegate, FString, Result, UCharacterData*, CharacterData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDeleteCharacterDelegate, FString, Result);

DECLARE_DYNAMIC_DELEGATE_OneParam(FCharactersLoadedDelegate, FString, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharactersChanged);

UCLASS()
class L3_PROJECT_API UCharacterManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	int MaxCharacterCount = 4;
	
	UPROPERTY(BlueprintAssignable)
	FCharactersChanged OnCharactersChanged;

	UPROPERTY(BlueprintReadOnly)
	bool bCharactersLoaded = false;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, UCharacterData*> CharactersData;

	UPROPERTY(BlueprintReadOnly)
	UCharacterData* SelectedCharacter;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable)
	void LoadCharacters(FCharactersLoadedDelegate Callback);
private:
	FCharactersLoadedDelegate CharactersLoadedCallback;
	
	UFUNCTION()
	void LoadCharactersSuccess(const FString& Data);

	UFUNCTION()
	void LoadCharactersFailed(const FString& ErrorMessage);
	
public:
	UFUNCTION(BlueprintCallable)
	void SaveCharacter(UCharacterData* TempCharacter, FSaveCharacterDelegate Callback);

private:
	FSaveCharacterDelegate SaveCharacterCallback;

	UFUNCTION()
	void SaveCharacterSuccess(const FString& Result);

	UFUNCTION()
	void SaveCharacterFailed(const FString& ErrorMessage);
	
public:
	UFUNCTION(BlueprintCallable)
	void CreateCharacter(FString Name, FCreateCharacterDelegate Callback);

private:
	FString CreatingCharacterId;
	FCreateCharacterDelegate CreateCharacterCallback;

	UFUNCTION()
	void CreateCharacterSuccess(const FString& Result);

	UFUNCTION()
	void CreateCharacterFailed(const FString& ErrorMessage);
	
public:
	UFUNCTION(BlueprintCallable)
	void DeleteCharacter(FString CharacterUUID, FDeleteCharacterDelegate Callback);

private:
	FString DeletingCharacterId;
	
	FDeleteCharacterDelegate DeleteCharacterCallback;

	UFUNCTION()
	void DeleteCharacterSuccess(const FString& Result);

	UFUNCTION()
	void DeleteCharacterFailed(const FString& ErrorMessage);
	
public:
	UFUNCTION(BlueprintCallable)
	void SelectCharacter(UCharacterData* Character);

private:
	UPROPERTY()
	FSuccess SuccessCallback;
	
	UPROPERTY()
	FFailed FailedCallback;
};
