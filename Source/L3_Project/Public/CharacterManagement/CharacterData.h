#pragma once
#include "Spells/SpellDataAsset.h"
#include "CharacterData.generated.h"

class USpellDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellChanged, USpellDataAsset*, Spell, uint8, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, USpellDataAsset*, Spell);

UCLASS(BlueprintType)
class L3_PROJECT_API UCharacterData : public UObject
{
	GENERATED_BODY()

public:
	UCharacterData() = default;
	
	UCharacterData(FString InUUID, FString InName) : UUID(InUUID), Name(InName), SelectedWeaponID(0), SelectedSpellsID(0){}
	
	UPROPERTY(BlueprintReadWrite)
	FString UUID;
	
	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	uint8 SelectedWeaponID;
	
	UPROPERTY(BlueprintReadWrite)
	int32 SelectedSpellsID;

	UPROPERTY(BlueprintAssignable)
	FOnSpellChanged OnSpellChanged;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponChanged OnWeaponChanged;
	
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
	USpellDataAsset* GetSpellAt(UObject* WorldContext, uint8 Index);
	
	UFUNCTION(BlueprintCallable)
	USpellDataAsset* GetWeapon(UObject* WorldContext);

	UFUNCTION(BlueprintCallable)
	bool IsCharacterValid(UObject* WorldContext);

	UFUNCTION(BlueprintCallable)
	bool HasSpell(USpellDataAsset* Spell);
};
