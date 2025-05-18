#pragma once
#include "Spells/SpellDataAsset.h"
#include "CharacterData.generated.h"

class USpellDataAsset;

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

	void SetUUID(FString NewUUID)
	{
		UUID = NewUUID;
	}
	
	void SetName(FString NewName);

	UFUNCTION(BlueprintCallable)
	void SelectWeapon(const USpellDataAsset* WeaponSpell);
	
	UFUNCTION(BlueprintCallable)
	void SelectSpell(const USpellDataAsset* SpellDataAsset, uint8 Index);

	UFUNCTION(BlueprintCallable)
	USpellDataAsset* GetSpellAt(UObject* WorldContext, uint8 Index);
	
	UFUNCTION(BlueprintCallable)
	USpellDataAsset* GetWeapon(UObject* WorldContext);
};
