#pragma once
#include "CharacterData.h"
#include "CharacterManagerSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FSaveCharacterDelegate, FString, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FCreateCharacterDelegate, FString, Result, UCharacterData*, CharacterData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDeleteCharacterDelegate, FString, Result);
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
	TMap<FString, UCharacterData*> CharactersData;

	UPROPERTY(BlueprintReadOnly)
	UCharacterData* SelectedCharacter;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable)
	void LoadCharacters();

	UFUNCTION(BlueprintCallable)
	void SaveCharacter(uint8 CharacterIndex, FSaveCharacterDelegate Callback);
	
	UFUNCTION(BlueprintCallable)
	void CreateCharacter(FString Name, FCreateCharacterDelegate Callback);

	UFUNCTION(BlueprintCallable)
	void DeleteCharacter(FString CharacterUUID, FDeleteCharacterDelegate Callback);

	UFUNCTION(BlueprintCallable)
	void SelectCharacter(UCharacterData* Character);
};
