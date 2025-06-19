// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellController.generated.h"

class UAimResultHolder;
struct FInputActionValue;
class UInputAction;
class ASpellAimer;
class USpellDataAsset;
class USpellController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellsChanged, USpellController*, SpellController);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCooldownReplicated, uint8, Index, float, NewCooldown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalCooldownChanged, bool, IsActive);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCastStartDelegate, USpellDataAsset*, Spell, int, SpellIndex);

UCLASS(Blueprintable)
class USpellControllerCastState : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool IsCasting{};

	UPROPERTY(BlueprintReadOnly)
	int SpellIndex{};

	UPROPERTY(BlueprintReadOnly)
	USpellDataAsset* Spell{};

	// Valid only on server side
	UPROPERTY(BlueprintReadOnly)
	UAimResultHolder* AimResult{};

	UPROPERTY(BlueprintReadOnly)
	double AnimationStartTime{};

	UPROPERTY(BlueprintReadOnly)
	double AnimationEndTime{};

	UPROPERTY(BlueprintReadOnly)
	int ComboIndex{};
};

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class L3_PROJECT_API USpellController : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpellController();
	
	UPROPERTY(BlueprintReadOnly)
	USpellControllerCastState* CastState;

	UPROPERTY(EditAnywhere)
	UInputAction* ValidateInput;

	UPROPERTY(EditAnywhere)
	UInputAction* CancelInput;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UInputAction*> SpellInputs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* AttachSocket;
	
	UPROPERTY(ReplicatedUsing=OnSpellDataReplicated, EditAnywhere, BlueprintReadWrite)
	TArray<USpellDataAsset*> SpellDatas;

	UPROPERTY(BlueprintReadOnly)
	TArray<float> RepCooldowns;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int MaxSpells = 7;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int WeaponSpellIndex = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int DashSpellIndex = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int HealSpellIndex = 2;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int SpellFirstIndex = 3;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadOnly)
	float GlobalCooldownValue = 0.5f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float CooldownsReplicationDelay = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USpellDataAsset* DashSpell;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USpellDataAsset* HealSpell;
	
	UPROPERTY(BlueprintAssignable)
	FOnSpellsChanged OnSpellDatasChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCastStartDelegate OnCastStart;

	UPROPERTY(BlueprintAssignable)
	FOnCooldownReplicated OnCooldownReplicatedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnGlobalCooldownChanged OnGlobalCooldownChanged;
	
	UFUNCTION(BlueprintCallable)
	USpellDataAsset* GetSpellData(int Index) const;

	UFUNCTION(BlueprintCallable)
	TArray<USpellDataAsset*> GetSpellDatas() const;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void TrySelectSpellRpc(int Index, USpellDataAsset* Spell);

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void SrvSelectSpell(int Index, USpellDataAsset* Spell, bool bShouldReplicate = true);
	
	void StartGlobalCooldown();
	void StartCooldown(int Index);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SrvResetAllCooldowns();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SendSpellCastResponse(int SpellIndex, UAimResultHolder* Result, USpellDataAsset* NextSpell);
	void SendSpellCastResponse(int SpellIndex, UAimResultHolder* Result);
	
	UFUNCTION(NetMulticast, Reliable)
	void SpellCastResponseMultiCastRpc(int SpellIndex, USpellDataAsset* Spell);

	UFUNCTION(Client, Reliable)
	void InvalidSpellCastResponseOwnerRpc();
	
	UFUNCTION(BlueprintCallable)
	void SrvOnAnimationCastSpellNotify(float Duration);
	
	UFUNCTION(BlueprintCallable)
	void OnAnimationEndedNotify();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCasting() const;

	bool CanCombo(const int SpellIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAiming() const;

	void ForceReplicateSpellDatas();
	
protected:	
	UFUNCTION(BlueprintCallable)
	void UpdateAttachSocket();
	
	UFUNCTION(BlueprintCallable)
	void ReplicateGlobalCooldown(const bool NewGlobalCooldown);

	UFUNCTION(BlueprintCallable)
	void ReplicateSpellDatas(const TArray<USpellDataAsset*>& NewSpellDatas);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsOwnerLocallyControlled() const;

	UFUNCTION(BlueprintImplementableEvent)
	bool CanStartAiming_BP(int SpellIndex) const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPredictedCast(int SpellIndex, UAimResultHolder* Result);

	UFUNCTION(BlueprintImplementableEvent)
	void OnInvalidCastResponse();
	
private:
	UPROPERTY()
	float GlobalCooldown = 0.0f;
	UPROPERTY(ReplicatedUsing=OnGlobalCooldownReplicated)
	bool bIsInGlobalCooldown;
	
	UPROPERTY()
	TArray<float> Cooldowns;
	
	UPROPERTY()
	TArray<ASpellAimer*> SpellAimers;

	ASpellAimer* ActiveAimer;

	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Inputs
	
	void SetupInputs();

	void OnSpellInputStarted(int Index);
	void OnSpellInputStopped(int Index);

	void OnValidateInputStarted();

	void OnCancelInputStarted();

#pragma endregion 
	
#pragma region Replication
	
	UFUNCTION()
	void OnSpellDataReplicated();

	UFUNCTION()
	void SrvReplicateCooldown(uint8 Index, float CooldownValue);
	
	UFUNCTION(Client, Reliable)
	void ReplicateCooldownRpc(uint8 Index, float CooldownValue);

	UFUNCTION(Client, Reliable)
	void ResetCooldownsRpc();
	
	UFUNCTION()
	void OnGlobalCooldownReplicated();

#pragma endregion 

	void StopAndCast(ASpellAimer* Aimer, int Index);
	
	void UpdateSpellAimers();
	
	void UpdateCooldowns(float DeltaTime);

#pragma region Utils
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInCooldown(int Index) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanStartAiming(int SpellIndex) const;
	
	bool TryGetSpellAimer(int Index, ASpellAimer*& OutAimer) const;

#pragma endregion 
	
#pragma region RPCs
	
	UFUNCTION(BlueprintCallable)
	void RequestSpellCastGenericResultToServer(int SpellIndex, UAimResultHolder* Result);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RequestSpellCastFromControllerRpc_Vector(int SpellIndex, USpellController* Caster, FVector Result);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RequestSpellCastFromControllerRpc_Actor(int SpellIndex, USpellController* Caster, AActor* Result);
	
#pragma endregion 
};