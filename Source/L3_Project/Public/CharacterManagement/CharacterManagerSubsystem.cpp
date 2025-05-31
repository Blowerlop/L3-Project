#include "CharacterManagerSubsystem.h"

#include "CharacterData.h"
#include "Database/DatabaseFunctions.h"
#include "Networking/BaseGameInstance.h"

void UCharacterManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CharactersData = {};
}

void UCharacterManagerSubsystem::LoadCharacters(FCharactersLoadedDelegate Callback)
{
	const auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
	const auto ClientData = GameInstance->SelfClientData;
	
	SuccessCallback.BindUFunction(this, "LoadCharactersSuccess");
	
	FailedCallback.BindUFunction(this, "LoadCharactersFailed");

	CharactersLoadedCallback = Callback;
	UDatabaseFunctions::GetAllCharacters(ClientData.UUID, UDatabaseFunctions::GetIdToken(), SuccessCallback, FailedCallback);
}

void UCharacterManagerSubsystem::OnFirebaseLogout()
{
	CharactersData.Empty();
}

void UCharacterManagerSubsystem::LoadCharactersSuccess(const FString& Data)
{
	SuccessCallback.Clear();
	FailedCallback.Clear();

	UE_LOG(LogTemp, Error, TEXT("Loaded characters: %s"), *Data);
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Data);
	if (TSharedPtr<FJsonObject> JsonResponse; FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
	{
		for (TTuple<FString, TSharedPtr<FJsonValue>> Value : JsonResponse->Values)
		{
			auto CharacterData = NewObject<UCharacterData>();
			CharacterData->SetUUID(Value.Key);

			const auto JsonObj = Value.Value->AsObject();
			if (JsonObj == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Character data for UUID %s is not valid."), *Value.Key);
				continue;
			}
			
			const auto Name = JsonObj->GetStringField(TEXT("Name"));
			const auto SelectedWeaponID = static_cast<uint8>(JsonObj->GetIntegerField(TEXT("WeaponID")));
			const auto SelectedSpellsID = JsonObj->GetIntegerField(TEXT("SelectedSpells"));
			
			CharacterData->SetName(Name);
			CharacterData->SelectedWeaponID = SelectedWeaponID;
			CharacterData->SelectedSpellsID = SelectedSpellsID;

			CharactersData.Add(Value.Key, CharacterData);
		}

		bCharactersLoaded = true;
		CharactersLoadedCallback.ExecuteIfBound("");
		OnCharactersChanged.Broadcast();
		return;
	}
	
	CharactersLoadedCallback.ExecuteIfBound("");
}

void UCharacterManagerSubsystem::LoadCharactersFailed(const FString& ErrorMessage)
{
	SuccessCallback.Clear();
	FailedCallback.Clear();
	
	UE_LOG(LogTemp, Error, TEXT("Characters loading failed: %s"), *ErrorMessage);
	CharactersLoadedCallback.ExecuteIfBound(ErrorMessage);
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

	const auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
	if (!IsValid(GameInstance))
	{
		Callback.ExecuteIfBound("SaveCharacter: GameInstance is not valid.");
		return;
	}

	const auto ClientData = GameInstance->SelfClientData;

	auto FieldArray = TArray<FString>();
	FieldArray.Add("WeaponID");
	FieldArray.Add("SelectedSpells");
	
	auto ValuesArray = TArray<int>();
	ValuesArray.Add(TempCharacter->SelectedWeaponID);
	ValuesArray.Add(TempCharacter->SelectedSpellsID);

	SuccessCallback.BindUFunction(this, "SaveCharacterSuccess");
	FailedCallback.BindUFunction(this, "SaveCharacterFailed");

	SaveCharacterCallback = Callback;
	
	UDatabaseFunctions::SetCharacterData_Int(ClientData.UUID, TempCharacter->UUID, FieldArray,
		ValuesArray, UDatabaseFunctions::GetIdToken(), SuccessCallback, FailedCallback);
}

void UCharacterManagerSubsystem::SaveCharacterSuccess(const FString& Result)
{
	SuccessCallback.Clear();
	FailedCallback.Clear();

	TSharedPtr<FJsonObject> JsonResponse;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Result);
	if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
	{
		SelectedCharacter->SelectedWeaponID = JsonResponse->GetIntegerField(TEXT("WeaponID"));
		SelectedCharacter->SelectedSpellsID = JsonResponse->GetIntegerField(TEXT("SelectedSpells"));

		SaveCharacterCallback.ExecuteIfBound("");
		OnCharactersChanged.Broadcast();
		return;
	}
	
	SaveCharacterCallback.ExecuteIfBound("Response is not valid!");
}

void UCharacterManagerSubsystem::SaveCharacterFailed(const FString& ErrorMessage)
{
	SuccessCallback.Clear();
	FailedCallback.Clear();

	SaveCharacterCallback.ExecuteIfBound(ErrorMessage);
}

void UCharacterManagerSubsystem::CreateCharacter(const FString Name, FCreateCharacterDelegate Callback)
{
	if (Name.IsEmpty())
	{
		Callback.ExecuteIfBound("Character name is invalid. Name cannot be empty.", nullptr);
		return;
	}

	const auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
	if (!IsValid(GameInstance))
	{
		Callback.ExecuteIfBound("GameInstance is not valid.", nullptr);
		return;
	}

	const auto ClientData = GameInstance->SelfClientData;

	SuccessCallback.BindUFunction(this, "CreateCharacterSuccess");
	FailedCallback.BindUFunction(this, "CreateCharacterFailed");
	
	CreateCharacterCallback = Callback;
	CreatingCharacterId = UDatabaseFunctions::CreateCharacter(ClientData.UUID, UDatabaseFunctions::GetIdToken(), Name,
		0, 0, SuccessCallback, FailedCallback);
}

void UCharacterManagerSubsystem::CreateCharacterSuccess(const FString& Result)
{
	SuccessCallback.Clear();
	FailedCallback.Clear();

	UE_LOG(LogTemp, Error, TEXT("Character created successfully: %s"), *Result);

	const auto CharacterData = NewObject<UCharacterData>();
	CharacterData->SetUUID(CreatingCharacterId);

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Result);
	if (TSharedPtr<FJsonObject> JsonResponse; FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
	{
		CharacterData->SetName(JsonResponse->GetStringField(TEXT("Name")));

		CharactersData.Add(CreatingCharacterId, CharacterData);
	
		CreateCharacterCallback.ExecuteIfBound("", CharacterData);
		OnCharactersChanged.Broadcast();

		CreatingCharacterId = "";
		return;
	}
	
	CreatingCharacterId = "";
	CreateCharacterCallback.ExecuteIfBound("Response not valid!", nullptr);
}

void UCharacterManagerSubsystem::CreateCharacterFailed(const FString& ErrorMessage)
{
	SuccessCallback.Clear();
	FailedCallback.Clear();

	CreatingCharacterId = "";
	CreateCharacterCallback.ExecuteIfBound(ErrorMessage, nullptr);
}

void UCharacterManagerSubsystem::DeleteCharacter(const FString CharacterUUID, FDeleteCharacterDelegate Callback)
{
	if (!CharactersData.Contains(CharacterUUID))
	{
		Callback.ExecuteIfBound("Character with UUID " + CharacterUUID + " not found locally.");
		return;
	}

	const auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
	const auto ClientData = GameInstance->SelfClientData;

	SuccessCallback.BindUFunction(this, "DeleteCharacterSuccess");
	FailedCallback.BindUFunction(this, "DeleteCharacterFailed");

	DeleteCharacterCallback = Callback;

	DeletingCharacterId = CharacterUUID;
	UDatabaseFunctions::DeleteCharacter(ClientData.UUID, CharacterUUID, UDatabaseFunctions::GetIdToken(), SuccessCallback, FailedCallback);
}

void UCharacterManagerSubsystem::DeleteCharacterSuccess(const FString& Result)
{
	SuccessCallback.Clear();
	FailedCallback.Clear();
	
	CharactersData.Remove(DeletingCharacterId);
	
	DeleteCharacterCallback.ExecuteIfBound("");
	OnCharactersChanged.Broadcast();

	DeletingCharacterId = "";
}

void UCharacterManagerSubsystem::DeleteCharacterFailed(const FString& ErrorMessage)
{
	SuccessCallback.Clear();
	FailedCallback.Clear();
	
	DeleteCharacterCallback.ExecuteIfBound(ErrorMessage);

	DeletingCharacterId = "";
}

void UCharacterManagerSubsystem::SelectCharacter(UCharacterData* Character)
{
	SelectedCharacter = Character;
}
