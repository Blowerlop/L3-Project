#include "CharacterData.h"

#include "Spells/SpellDataAsset.h"
#include "Spells/SpellDatabase.h"



void UCharacterData::SetName(FString NewName)
{
	Name = NewName;
}

void UCharacterData::SelectWeapon(USpellDataAsset* WeaponSpell)
{
	SelectedWeaponID = IsValid(WeaponSpell) ? WeaponSpell->AssetID : 0;
	OnWeaponChanged.Broadcast(WeaponSpell);
}

void UCharacterData::SelectSpell(USpellDataAsset* SpellDataAsset, const uint8 Index)
{
	if (Index < 0 || Index >= NumSpells)
	{
		UE_LOG(LogTemp, Error, TEXT("FCharacterData::SelectSpell: Index out of bounds! Index: %d"), Index);
		return;
	}
	
	// uint8 is 1 byte, 8 bits
	const auto ShiftAmount = Index * 8;

	// Clear the bits at given index
	SelectedSpellsID &= ~(GBit_Mask_8 << ShiftAmount);

	if (IsValid(SpellDataAsset))
	{
		// Assign new id at given index
		SelectedSpellsID |= (SpellDataAsset->AssetID << ShiftAmount);
	}

	OnSpellChanged.Broadcast(SpellDataAsset, Index);
}

USpellDataAsset* UCharacterData::GetSpellAt(UObject* WorldContext, const uint8 Index)
{
	if (Index < 0 || Index >= NumSpells)
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

static bool IsSpellConditionMet(USpellDataAsset* Spell, USpellDataAsset* WeaponSpell)
{
	if (!IsValid(Spell) || !IsValid(WeaponSpell)) return false;

	// If the spell has a weapon condition, check if it matches the selected weapon
	if (Spell->WeaponSpellCondition && Spell->WeaponSpellCondition->AssetID != WeaponSpell->AssetID)
	{
		return false;
	}

	return true;
}

bool UCharacterData::IsCharacterValid(UObject* WorldContext)
{
	const auto WeaponSpell = GetWeapon(WorldContext);
	if (!IsValid(WeaponSpell)) return false;
	
	for(int i = 0; i < NumSpells; ++i)
	{
		const auto Spell = GetSpellAt(WorldContext, i);
		
		if (!IsValid(Spell)) return false;
		if (!IsSpellConditionMet(Spell, WeaponSpell)) return false;
	}

	return true;
}

bool UCharacterData::HasSpell(USpellDataAsset* Spell)
{
	if (!IsValid(Spell)) return false;

	if (SelectedWeaponID == Spell->AssetID) return true;

	// for loop is not the most efficient way, but it's simple and readable.
	for(int i = 0; i < NumSpells; ++i)
	{
		const auto SpellID = (SelectedSpellsID >> (i * 8)) & GBit_Mask_8;
		if (SpellID == Spell->AssetID)
		{
			return true;
		}
	}

	return false;
}
