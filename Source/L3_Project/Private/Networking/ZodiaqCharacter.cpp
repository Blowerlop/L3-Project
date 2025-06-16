#include "Networking/ZodiaqCharacter.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Networking/BaseGameInstance.h"
#include "Networking/ZodiaqGameMode.h"
#include "Networking/ZodiaqPlayerState.h"
#include "Spells/SpellController.h"
#include "Spells/SpellDatabase.h"

void AZodiaqCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// UE_LOG(LogTemp, Warning, TEXT("AZodiaqCharacter::BeginPlay called for %s"), *GetName());
	if (UKismetSystemLibrary::IsServer(this))
	{
		const auto ZodiaqPlayerState = GetPlayerState<AZodiaqPlayerState>();

		if (IsValid(ZodiaqPlayerState) && (!bIsLoaded && ZodiaqPlayerState->bIsCharacterLoaded))
		{
			LoadCharacterFromPlayerState(ZodiaqPlayerState);
		}
	}
}

void AZodiaqCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	const auto GameInstance = GetGameInstance<UBaseGameInstance>();
	if (IsValid(GameInstance))
	{
		GameInstance->NotifyCharacterDestroyed(this);
	}
}

void AZodiaqCharacter::LoadCharacterFromPlayerState(AZodiaqPlayerState* ZodiaqPlayerState)
{
	const auto PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController)) return;
	
	InitSpells(PlayerController, ZodiaqPlayerState);

	LoadCharacterFromPlayerState_BP(ZodiaqPlayerState);
	
	bIsLoaded = true;
	
	if (const auto GameInstance = GetGameInstance<UBaseGameInstance>(); IsValid(GameInstance))
	{
		GameInstance->NotifyCharacterLoaded(this);
	}
}

FClientData AZodiaqCharacter::GetClientData() const
{
	if (UKismetSystemLibrary::IsServer(this))
	{
		const auto ZodiaqPlayerState = GetPlayerState<AZodiaqPlayerState>();
		if (IsValid(ZodiaqPlayerState))
		{
			return ZodiaqPlayerState->ClientData;
		}
	}
	else
	{
		const auto GameInstance = GetGameInstance<UBaseGameInstance>();
		if (IsValid(GameInstance))
		{
			return GameInstance->SelfClientData;
		}
	}

	return {};
}

void AZodiaqCharacter::OnRep_InstanceId()
{
	OnInstanceIdChanged.Broadcast(this, InstanceId);
}

FString AZodiaqCharacter::GetIdentifier_Implementation()
{
	return GetClientData().UUID;
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
	SpellController->SrvSelectSpell(SpellController->DashSpellIndex, SpellController->DashSpell, false);
	SpellController->SrvSelectSpell(SpellController->HealSpellIndex, SpellController->HealSpell, false);
	
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

void AZodiaqCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AZodiaqCharacter, InstanceId, COND_OwnerOnly);
}


