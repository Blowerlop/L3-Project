#pragma once
#include "Spells/SpellDataAsset.h"
#include "CharacterData.generated.h"

class USpellDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellChanged, USpellDataAsset*, Spell, uint8, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, USpellDataAsset*, Spell);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkinChanged, int, Skin);

UCLASS(BlueprintType)
class L3_PROJECT_API UCharacterData : public UObject
{
	GENERATED_BODY()

public:
	static constexpr uint8 GBit_Mask_8 = 0xFF /* 255 */;
	static constexpr uint8 NumSpells = 4;
	
	UCharacterData() = default;
	
	UCharacterData(FString InUUID, FString InName) : UUID(InUUID), Name(InName)	, SelectedSkin(0), SelectedWeaponID(0), SelectedSpellsID(0) {}
	
	UPROPERTY(BlueprintReadWrite)
	FString UUID{};
	
	UPROPERTY(BlueprintReadWrite)
	FString Name{};

	UPROPERTY(BlueprintReadWrite)
	int SelectedSkin;
	
	UPROPERTY(BlueprintReadWrite)
	uint8 SelectedWeaponID{};
	
	UPROPERTY(BlueprintReadWrite)
	int32 SelectedSpellsID{};

	UPROPERTY(BlueprintAssignable)
	FOnSpellChanged OnSpellChanged{};

	UPROPERTY(BlueprintAssignable)
	FOnWeaponChanged OnWeaponChanged{};

	UPROPERTY(BlueprintAssignable)
	FOnSkinChanged OnSkinChanged{};
	
	void SetUUID(FString NewUUID)
	{
		UUID = NewUUID;
	}
	
	void SetName(FString NewName);

	UFUNCTION(BlueprintCallable)
	void SelectWeapon(USpellDataAsset* WeaponSpell);
	
	UFUNCTION(BlueprintCallable)
	void SelectSpell(USpellDataAsset* SpellDataAsset, uint8 Index);

	UFUNCTION(BlueprintCallable)
	void SelectSkin(int NewSkin);
	
	UFUNCTION(BlueprintCallable)
	USpellDataAsset* GetSpellAt(UObject* WorldContext, uint8 Index);
	
	UFUNCTION(BlueprintCallable)
	USpellDataAsset* GetWeapon(UObject* WorldContext);

	UFUNCTION(BlueprintCallable)
	bool IsCharacterValid(UObject* WorldContext);

	UFUNCTION(BlueprintCallable)
	bool HasSpell(USpellDataAsset* Spell);
};

USTRUCT(BlueprintType)
struct FSerializableCharacterData
{
	GENERATED_BODY()

	FSerializableCharacterData() = default;
	FSerializableCharacterData(const uint8 InSelectedWeaponID, const int32 InSelectedSpellsID, const int InSelectedSkin)
		: SelectedWeaponID(InSelectedWeaponID), SelectedSpellsID(InSelectedSpellsID), SelectedSkin(InSelectedSkin) {}

	UPROPERTY(BlueprintReadWrite)
	uint8 SelectedWeaponID{};
	
	UPROPERTY(BlueprintReadWrite)
	int32 SelectedSpellsID{};

	UPROPERTY(BlueprintReadWrite)
	int SelectedSkin{};
};
