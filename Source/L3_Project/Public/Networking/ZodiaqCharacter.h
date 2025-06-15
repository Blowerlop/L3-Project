#pragma once
#include "GameFramework/Character.h"
#include "Vitals/IAliveState.h"
#include "Vitals/IInstigatorChainElement.h"
#include "ZodiaqCharacter.generated.h"

struct FClientData;
class AZodiaqPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInstanceIdChanged, AZodiaqCharacter*, Character, int, NewInstanceId);

UCLASS(BlueprintType, Blueprintable)
class AZodiaqCharacter : public ACharacter, public IAliveState, public IInstigatorChainElement
{
	GENERATED_BODY()

	bool bIsLoaded = false;
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void InitSpells(APlayerController* PlayerController, AZodiaqPlayerState* ZodiaqPlayerState);

public:
	UPROPERTY(BlueprintAssignable)
	FOnInstanceIdChanged OnInstanceIdChanged;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_InstanceId)
	int InstanceId{};
	
	UFUNCTION(BlueprintCallable)
	bool TeleportFromBP(const FVector& Location, const FRotator& Rotation, bool bIsATest = false, bool bNoCheck = false);
	
	void LoadCharacterFromPlayerState(AZodiaqPlayerState* ZodiaqPlayerState);

	UFUNCTION(BlueprintImplementableEvent)
	void LoadCharacterFromPlayerState_BP(AZodiaqPlayerState* ZodiaqPlayerState);

	UFUNCTION(BlueprintCallable)
	FClientData GetClientData() const;

	UFUNCTION()
	void OnRep_InstanceId();

	virtual FString GetIdentifier_Implementation() override;
};
