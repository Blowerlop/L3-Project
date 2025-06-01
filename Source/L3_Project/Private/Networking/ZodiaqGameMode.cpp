// Fill out your copyright notice in the Description page of Project Settings.

#include "Networking/ZodiaqGameMode.h"

#include "CharacterManagement/CharacterManagerSubsystem.h"
#include "Database/DatabaseFunctions.h"
#include "Kismet/GameplayStatics.h"
#include "Networking/BaseGameInstance.h"
#include "Networking/InstancesManagerSubsystem.h"
#include "Networking/SessionsManagerSubsystem.h"
#include "Networking/ZodiaqCharacter.h"
#include "Networking/ZodiaqPlayerState.h"

void AZodiaqGameMode::OnPlayerStateEndPlay(const AZodiaqPlayerState* PlayerState) const
{
	OnClientDestroyedDelegate.Broadcast(PlayerState->ClientData);
}

FString AZodiaqGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
                                       const FString& Options, const FString& Portal)
{
	UE_LOG(LogTemp, Warning, TEXT("InitNewPlayer called with Options: %s"), *Options);
	
	if (UInstancesManagerSubsystem::CurrentInstanceSettings.SessionID == 0 && !IsRunningDedicatedServer())
		return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	AZodiaqPlayerState* PlayerState;
	FString UUID{};
	FString CUUID{};
	FString Name{};
	
	const auto IsListen = Options.Contains("?Listen", ESearchCase::IgnoreCase);

	UE_LOG(LogTemp, Warning, TEXT("ListenOption: %hhd"), IsListen);
	
	if (IsListen)
	{
		const auto GameInstance = GetGameInstance<UBaseGameInstance>();
		if (!IsValid(GameInstance))
		{
			return "GameInstance is not UBaseGameInstance. Something is wrong.";
		}

		PlayerState = NewPlayerController->GetPlayerState<AZodiaqPlayerState>();
		if (PlayerState == nullptr)
		{
			return "PlayerState is null. Either it is not AZodiaqPlayerState or this is Unreal bullshit. Game is going to crash.";
		}
		
		const auto ClientData = GameInstance->SelfClientData;

		UUID = ClientData.UUID;
		Name = ClientData.Name;

		const auto CharacterManager = GameInstance->GetSubsystem<UCharacterManagerSubsystem>();
		CUUID = CharacterManager->SelectedCharacter->UUID;
	}
	else
	{
		UUID = UGameplayStatics::ParseOption(Options, UBaseGameInstance::UUIDConnectOptionsKey);
		if (UUID.IsEmpty())
		{
			return "Given UUID is incorrect. Try to reconnect.";
		}

		CUUID = UGameplayStatics::ParseOption(Options, UBaseGameInstance::CharacterUUIDConnectOptionsKey);
		if (CUUID.IsEmpty())
		{
			return "Selected Character is invalid. Try to reconnect.";
		}

		Name = UGameplayStatics::ParseOption(Options, UBaseGameInstance::UserNameConnectOptionsKey);
		if (Name.IsEmpty())
		{
			return "Given Name is incorrect. Try to reconnect.";
		}

		PlayerState = NewPlayerController->GetPlayerState<AZodiaqPlayerState>();
		if (PlayerState == nullptr)
		{
			return "Server error. Try to reconnect.";
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Player %s connected with UUID %s"), *Name, *UUID);
	PlayerState->ClientData = FClientData(UUID, Name);

	OnClientSpawnedDelegate.Broadcast(PlayerState->ClientData);

	GetCharacterCallbackDelegate.BindUFunction(this, "GetCharacterCallback");
	
	UDatabaseFunctions::GetCharacterData(NewPlayerController, UUID, CUUID, UDatabaseFunctions::GetIdToken(), GetCharacterCallbackDelegate);
	
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AZodiaqGameMode::DisconnectPlayer(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return;

	// Fuck this engine, can't find a method to just disconnect a client. Traveling to another world instead
	PlayerController->ClientTravel("/Game/_Project/000-Game/Maps/MainMenuMap", TRAVEL_Absolute);
}

void AZodiaqGameMode::GetCharacterCallback(const bool CharacterValid, const FString& Response, APlayerController* PlayerController)
{
	GetCharacterCallbackDelegate.Clear();

	if (!CharacterValid)
	{
		UE_LOG(LogTemp, Error, TEXT("GetCharacterCallback: Character is not valid %s"), *Response);
		DisconnectPlayer(PlayerController);
		return;
	}

	const auto PlayerState = PlayerController->GetPlayerState<AZodiaqPlayerState>();
	if (PlayerState == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetCharacterCallback: PlayerState is not valid %s"), *Response);
		DisconnectPlayer(PlayerController);
		return;
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
	if (TSharedPtr<FJsonObject> JsonResponse; FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
	{
		const auto SelectedWeapon = JsonResponse->GetIntegerField(TEXT("WeaponID"));
		const auto SelectedSpells = JsonResponse->GetIntegerField(TEXT("SelectedSpells"));

		if (SelectedWeapon <= 0 || SelectedSpells <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("GetCharacterCallback: Invalid weapon or spells ID: WeaponID: %d, SpellsID: %d"),
			       SelectedWeapon, SelectedSpells);
			// Playe rhave missing spells
			DisconnectPlayer(PlayerController);
			return;
		}
		
		PlayerState->LoadCharacter(FSerializableCharacterData(SelectedWeapon, SelectedSpells));
		
		UE_LOG(LogTemp, Warning, TEXT("Player %s validated with CharacterData: WeaponID: %d, SpellsID: %d"),
		       *PlayerState->ClientData.Name, PlayerState->ClientData.CharacterData.SelectedWeaponID, PlayerState->ClientData.CharacterData.SelectedSpellsID);
		
		const auto Character = PlayerController->GetPawn<AZodiaqCharacter>();
		// Character can be invalid at this time, it will try to load itself on begin play if it can't load here.
		if (IsValid(Character))
		{
			Character->LoadCharacterFromPlayerState(PlayerState);	
		}
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("GetCharacterCallback: can't deserialize response: %s"), *Response);
	// Can't deserialize response, something went wrong
	DisconnectPlayer(PlayerController);
}
