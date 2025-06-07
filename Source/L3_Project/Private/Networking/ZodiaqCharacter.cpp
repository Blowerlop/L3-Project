#include "Networking/ZodiaqCharacter.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Networking/ZodiaqGameMode.h"
#include "Networking/ZodiaqPlayerState.h"
#include "Spells/SpellController.h"
#include "Spells/SpellDatabase.h"

void AZodiaqCharacter::BeginPlay()
{
	Super::BeginPlay();

	// UE_LOG(LogTemp, Warning, TEXT("AZodiaqCharacter::BeginPlay called for %s"), *GetName());
	if (!UKismetSystemLibrary::IsServer(this)) return;

	const auto ZodiaqPlayerState = GetPlayerState<AZodiaqPlayerState>();

	if (!IsValid(ZodiaqPlayerState))
	{
		// UE_LOG(LogTemp, Error, TEXT("AZodiaqCharacter::BeginPlay: PlayerState is not valid for %s"), *GetName());
		return;
	}
	
	if (!bIsLoaded && ZodiaqPlayerState->bIsCharacterLoaded)
	{
		LoadCharacterFromPlayerState(ZodiaqPlayerState);
	}
}

void AZodiaqCharacter::LoadCharacterFromPlayerState(AZodiaqPlayerState* ZodiaqPlayerState)
{
	const auto PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController)) return;
	
	InitSpells(PlayerController, ZodiaqPlayerState);

	LoadCharacterFromPlayerState_BP(ZodiaqPlayerState);
	
	bIsLoaded = true;
}

void AZodiaqCharacter::InitSpells(APlayerController* PlayerController, AZodiaqPlayerState* ZodiaqPlayerState)
{
	const auto SpellController = GetComponentByClass<USpellController>();
	if (!IsValid(SpellController)) return;
	
	const auto SelectedCharacter = ZodiaqPlayerState->ClientData.CharacterData;
	if (SelectedCharacter.SelectedWeaponID == 0) return;
	
	const auto SpellDatabase = GetGameInstance()->GetSubsystem<USpellDatabase>();

	const auto Weapon = SpellDatabase->DataAssets.FindRef(SelectedCharacter.SelectedWeaponID);
	if (!IsValid(Weapon))
	{
		UE_LOG(LogTemp, Error, TEXT("Client %s tried to connect with invalid weapon ID %d"),
			   *ZodiaqPlayerState->ClientData.Name, SelectedCharacter.SelectedWeaponID);
		AZodiaqGameMode::DisconnectPlayer(PlayerController);
		return;
	}

	// bShouldReplicate false. Replicate everything after every spell is assigned.
	SpellController->SrvSelectSpell(SpellController->WeaponSpellIndex, Weapon, false);
	
	const auto SpellsCount = SpellController->MaxSpells - SpellController->SpellFirstIndex;
	
	for(auto i = 0; i < SpellsCount; i++)
	{
		const auto SpellID = static_cast<uint8>(SelectedCharacter.SelectedSpellsID >> (i * 8) & UCharacterData::GBit_Mask_8);
		
		const auto Spell = SpellDatabase->DataAssets.FindRef(SpellID);
		if (!IsValid(Spell))
		{
			UE_LOG(LogTemp, Error, TEXT("Client %s tried to connect with invalid spell ID %d at index %d"),
				   *ZodiaqPlayerState->ClientData.Name, SpellID, i);
			AZodiaqGameMode::DisconnectPlayer(PlayerController);
			continue;
		}
		
		SpellController->SrvSelectSpell(SpellController->SpellFirstIndex + i, Spell, false);
	}

	SpellController->ForceReplicateSpellDatas();
	
	UE_LOG(LogTemp, Log, TEXT("Character spells (count %d) initialized for Client %s connected with UUID %s"),
		   SpellsCount, *ZodiaqPlayerState->ClientData.Name, *ZodiaqPlayerState->ClientData.UUID);
}

bool AZodiaqCharacter::TeleportFromBP(const FVector& Location, const FRotator& Rotation, const bool bIsATest, const bool bNoCheck)
{
	return TeleportTo(Location, Rotation, bIsATest, bNoCheck); 
}
