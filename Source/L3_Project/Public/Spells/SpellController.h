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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellsChanged, USpellController*, SpellController);
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
	static constexpr int Max_Spells = 4;
	
	USpellController();
	

	UPROPERTY(BlueprintReadOnly)
	USpellControllerCastState* CastState;

	UPROPERTY(EditAnywhere)
	UInputAction* ValidateInput;

	UPROPERTY(EditAnywhere)
	UInputAction* CancelInput;
	
	UPROPERTY(EditAnywhere)
	TArray<UInputAction*> SpellInputs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* AttachSocket;
	
	UPROPERTY(ReplicatedUsing=OnSpellDataReplicated, EditAnywhere, BlueprintReadWrite)
	TArray<USpellDataAsset*> SpellDatas;

	UPROPERTY(BlueprintAssignable)
	FOnSpellsChanged OnSpellDatasChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCastStartDelegate OnCastStart;
	
	UFUNCTION(BlueprintCallable)
	USpellDataAsset* GetSpellData(int Index) const;

	UFUNCTION(BlueprintCallable)
	TArray<USpellDataAsset*> GetSpellDatas() const;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void TrySelectSpellRpc(int Index, USpellDataAsset* Spell);

	void StartGlobalCooldown();
	void StartCooldown(int Index);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SendSpellCastResponse(int SpellIndex, UAimResultHolder* Result, USpellDataAsset* NextSpell);
	void SendSpellCastResponse(int SpellIndex, UAimResultHolder* Result);
	
	UFUNCTION(NetMulticast, Reliable)
	void SpellCastResponseMultiCastRpc(int SpellIndex, USpellDataAsset* Spell);
	
	UFUNCTION(BlueprintCallable)
	void SrvOnAnimationCastSpellNotify();
	
	UFUNCTION(BlueprintCallable)
	void OnAnimationEndedNotify();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCasting() const;

	bool CanCombo(const int SpellIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAiming() const;

protected:
	UPROPERTY(Blueprintable, EditAnywhere)
	float GlobalCooldownValue = 0.5f;
	
	UFUNCTION(BlueprintCallable)
	void UpdateAttachSocket();

	UFUNCTION(BlueprintCallable)
	void ReplicateCooldowns(const TArray<int>& NewCooldowns);

	UFUNCTION(BlueprintCallable)
	void ReplicateGlobalCooldown(const bool NewGlobalCooldown);

	UFUNCTION(BlueprintCallable)
	void ReplicateSpellDatas(const TArray<USpellDataAsset*>& NewSpellDatas);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsOwnerLocallyControlled() const;

	UFUNCTION(BlueprintImplementableEvent)
	bool CanStartAiming_BP(int SpellIndex) const;
	
private:
	UPROPERTY()
	float GlobalCooldown = 0.0f;
	UPROPERTY(ReplicatedUsing=OnGlobalCooldownReplicated)
	bool bIsInGlobalCooldown;
	
	UPROPERTY()
	TArray<float> Cooldowns;
	UPROPERTY(ReplicatedUsing=OnCooldownsReplicated)
	TArray<int> RepCooldowns;

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
	void OnCooldownsReplicated();

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