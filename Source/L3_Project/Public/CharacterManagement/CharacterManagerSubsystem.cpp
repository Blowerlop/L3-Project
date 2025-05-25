#include "CharacterManagerSubsystem.h"

#include "CharacterData.h"

void UCharacterManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CharactersData = {};
}

void UCharacterManagerSubsystem::LoadCharacters()
{
	const auto CharacterUUID = FString::Printf(TEXT("C:%d"), CharactersData.Num());

	const auto DefaultCharacter = NewObject<UCharacterData>();
	DefaultCharacter->SetUUID(CharacterUUID);
	DefaultCharacter->SetName("Default Character");
	
	CharactersData.Add(CharacterUUID, DefaultCharacter);

	OnCharactersChanged.Broadcast();
}

void UCharacterManagerSubsystem::SaveCharacter(UCharacterData* TempCharacter, FSaveCharacterDelegate Callback)
{
	if (!IsValid(TempCharacter))
	{
		Callback.ExecuteIfBound("Character data is invalid. Cannot save character.");
		return;
	}
	
	const auto Character = CharactersData.FindRef(TempCharacter->UUID);
	if (!IsValid(Character))
	{
		Callback.ExecuteIfBound("Character with UUID " + TempCharacter->UUID + " not found locally.");
		return;
	}

	// Do it after firebase response
	Character->SelectedSpellsID = TempCharacter->SelectedSpellsID;
	Character->SelectedWeaponID = TempCharacter->SelectedWeaponID;
	
	// todo: call firebase function to save character data
	Callback.ExecuteIfBound("");
}

void UCharacterManagerSubsystem::CreateCharacter(const FString Name, FCreateCharacterDelegate Callback)
{
	// todo: call firebase function to create character data

	if (Name.IsEmpty())
	{
		Callback.ExecuteIfBound("Character name is invalid. Name cannot be empty.", nullptr);
		return;
	}
	
	const auto CharacterUUID = FString::Printf(TEXT("C:%d"), FMath::RandRange(0, 9999999));

	const auto CharacterData = NewObject<UCharacterData>();
	CharacterData->SetUUID(CharacterUUID);
	CharacterData->SetName(Name);
	
	CharactersData.Add(CharacterUUID, CharacterData);
	Callback.ExecuteIfBound("", CharacterData);

	OnCharactersChanged.Broadcast();
}

void UCharacterManagerSubsystem::DeleteCharacter(const FString CharacterUUID, FDeleteCharacterDelegate Callback)
{
	if (!CharactersData.Contains(CharacterUUID))
	{
		Callback.ExecuteIfBound("Character with UUID " + CharacterUUID + " not found locally.");
		return;
	}

	// todo: call firebase function to delete character data
	CharactersData.Remove(CharacterUUID);
	Callback.ExecuteIfBound("");
	
	OnCharactersChanged.Broadcast();
}

void UCharacterManagerSubsystem::SelectCharacter(UCharacterData* Character)
{
	SelectedCharacter = Character;
}
