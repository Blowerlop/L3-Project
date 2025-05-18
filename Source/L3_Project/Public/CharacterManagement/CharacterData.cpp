#include "CharacterData.h"

#include "Spells/SpellDataAsset.h"
#include "Spells/SpellDatabase.h"

constexpr uint8 GBit_Mask_8 = 0xFF /* 255 */;

void UCharacterData::SetName(FString NewName)
{
	Name = NewName;
}

void UCharacterData::SelectWeapon(const USpellDataAsset* WeaponSpell)
{
	if (WeaponSpell == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FCharacterData::SelectWeapon: WeaponSpell is null!"));
		return;
	}
	
	SelectedWeaponID = WeaponSpell->AssetID;
}

void UCharacterData::SelectSpell(const USpellDataAsset* SpellDataAsset, const uint8 Index)
{
	if (Index < 0 || Index > 3)
	{
		UE_LOG(LogTemp, Error, TEXT("FCharacterData::SelectSpell: Index out of bounds! Index: %d"), Index);
		return;
	}
	
	// uint8 is 1 byte, 8 bits
	const auto ShiftAmount = Index * 8;

	// Clear the bits at given index
	SelectedSpellsID &= ~(GBit_Mask_8 << ShiftAmount);

	// Assign new id at given index
	SelectedSpellsID |= (SpellDataAsset->AssetID << ShiftAmount);
}

USpellDataAsset* UCharacterData::GetSpellAt(UObject* WorldContext, const uint8 Index)
{
	if (Index < 0 || Index > 3)
	{
		UE_LOG(LogTemp, Error, TEXT("FCharacterData::GetSpellAt: Index out of bounds! Index: %d"), Index);
		return nullptr;
	}

	if(!IsValid(WorldContext))
	{
		UE_LOG(LogTemp, Error, TEXT("FCharacterData::GetSpellAt: WorldContext is not valid!"));
		return nullptr;
	}

	const auto World = WorldContext->GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("FCharacterData::GetWeapon: World is not valid!"));
		return nullptr;
	}
	
	const auto SpellID = (SelectedSpellsID >> (Index * 8)) & GBit_Mask_8;
	
	return World->GetGameInstance()->GetSubsystem<USpellDatabase>()->DataAssets.FindRef(SpellID);
}

USpellDataAsset* UCharacterData::GetWeapon(UObject* WorldContext)
{
	if (!IsValid(WorldContext))
	{
		UE_LOG(LogTemp, Error, TEXT("FCharacterData::GetWeapon: WorldContext is not valid!"));
		return nullptr;
	}
	
	const auto World = WorldContext->GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("FCharacterData::GetWeapon: World is not valid!"));
		return nullptr;
	}

	
	return World->GetGameInstance()->GetSubsystem<USpellDatabase>()->DataAssets.FindRef(SelectedWeaponID);
}
