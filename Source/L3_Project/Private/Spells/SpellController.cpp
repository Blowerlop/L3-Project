// Fill out your copyright notice in the Description page of Project Settings.

#include "Spells/SpellController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InSceneManagersRefs.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Spells/SpellAimer.h"
#include "Spells/SpellDataAsset.h"
#include "Spells/SpellManager.h"
#include "Spells/Results/ActorAimResultHolder.h"
#include "Spells/Results/VectorAimResultHolder.h"

USpellController::USpellController()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void USpellController::SendSpellCastResponse(const int SpellIndex, UAimResultHolder* Result, USpellDataAsset* NextSpell)
{
	if (!UKismetSystemLibrary::IsServer(this)) return;
	
	CastState->AimResult = Result;

	if (!NextSpell->bHasCombo)
	{
		StartCooldown(SpellIndex);
	}

	SpellCastResponseMultiCastRpc(SpellIndex, NextSpell);
}

void USpellController::SendSpellCastResponse(int SpellIndex, UAimResultHolder* Result)
{
	const auto Spell = GetSpellData(SpellIndex);
	if (!IsValid(Spell))
	{
		UE_LOG(LogTemp, Error, TEXT("Spell at index %d is not valid"), SpellIndex);
		return;
	}
	
	SendSpellCastResponse(SpellIndex, Result, Spell);
}

void USpellController::SpellCastResponseMultiCastRpc_Implementation(const int SpellIndex, USpellDataAsset* Spell)
{
	CastState->SpellIndex = SpellIndex;
	CastState->IsCasting = true;
	
    CastState->Spell = Spell;
    
    const auto StartTime = GetWorld()->GetTimeSeconds();
    	
    CastState->AnimationStartTime = StartTime;
    CastState->AnimationEndTime = StartTime + CastState->Spell->GetMontageSectionLength(CastState->Spell->ComboIndex);
    
	OnCastStart.Broadcast(CastState->Spell, CastState->SpellIndex);
}

void USpellController::InvalidSpellCastResponseOwnerRpc_Implementation()
{
	if (UKismetSystemLibrary::IsServer(this)) return;
	
	CastState->IsCasting = false;
	
	OnInvalidCastResponse();
}

void USpellController::SrvOnAnimationCastSpellNotify(float Duration)
{
	if (!UKismetSystemLibrary::IsServer(this)) return;
	
	const auto InSceneManagers = GetWorld()->GetGameInstance()->GetSubsystem<UInSceneManagersRefs>();
	const auto SpellManager = InSceneManagers->GetManager<ASpellManager>();
	
	SpellManager->TryCastSpell(CastState->Spell, GetOwner(), CastState->AimResult, Duration);
}

void USpellController::OnAnimationEndedNotify()
{
	CastState->IsCasting = false;
	
	if (!UKismetSystemLibrary::IsServer(this)) return;

	if (CastState->Spell != nullptr && CastState->Spell->bHasCombo)
	{
		StartCooldown(CastState->SpellIndex);
	}
}

bool USpellController::IsCasting() const
{
	if (!IsValid(CastState))
	{
		UE_LOG(LogTemp, Error, TEXT("CastState is not valid"));
			return true;
	}
	
	return CastState->IsCasting;
}

bool USpellController::CanCombo(const int SpellIndex) const
{
	if (!IsValid(CastState))
	{
		UE_LOG(LogTemp, Error, TEXT("CastState is not valid"));
		return false;
	}

	return CastState->Spell->bHasCombo && SpellIndex == CastState->SpellIndex;
}

bool USpellController::IsAiming() const
{
	return SpellAimers.ContainsByPredicate([](const ASpellAimer* Aimer) { return Aimer && Aimer->bIsAiming; });
}

void USpellController::ForceReplicateSpellDatas()
{
	ReplicateSpellDatas(SpellDatas);
}

USpellDataAsset* USpellController::GetSpellData(const int Index) const
{
	if (Index < 0 || Index >= SpellDatas.Num()) return nullptr;

	return SpellDatas[Index];
}

TArray<USpellDataAsset*> USpellController::GetSpellDatas() const
{
	return SpellDatas;
}

void USpellController::BeginPlay()
{
	Super::BeginPlay();

	if (IsOwnerLocallyControlled())
	{
		UpdateAttachSocket();
		SetupInputs();
	}

	CastState = NewObject<USpellControllerCastState>();
	CastState->SpellIndex = -1;

	RepCooldowns.SetNum(MaxSpells);

	if (SpellDatas.Num() > 0)
	{
		UpdateSpellAimers();
	}
	
	if (!UKismetSystemLibrary::IsServer(this)) return;
	
	Cooldowns.SetNum(MaxSpells);
	
	SpellDatas.SetNum(MaxSpells);

	ReplicateSpellDatas(SpellDatas);
}

void USpellController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!UKismetSystemLibrary::IsServer(this)) return;
	
	UpdateCooldowns(DeltaTime);
}

void USpellController::ReplicateGlobalCooldown(const bool NewGlobalCooldown)
{
	bIsInGlobalCooldown = NewGlobalCooldown;

	// For some reasons C++ RepUsing doesn't trigger replication method on listen server, but blueprint RepNotify does
	if (IsNetMode(NM_ListenServer) && IsOwnerLocallyControlled())
	{
		OnGlobalCooldownReplicated();
	}
}

void USpellController::ReplicateSpellDatas(const TArray<USpellDataAsset*>& NewSpellDatas)
{
	SpellDatas = NewSpellDatas;

	// For some reasons C++ RepUsing doesn't trigger replication method on listen server, but blueprint RepNotify does
	if (IsNetMode(NM_ListenServer) && IsOwnerLocallyControlled())
	{
		OnSpellDataReplicated();
	}
}

void USpellController::OnSpellDataReplicated()
{
	UpdateSpellAimers();
	OnSpellDatasChanged.Broadcast(this);
}

void USpellController::SrvReplicateCooldown(uint8 Index, float CooldownValue)
{
	if (IsOwnerLocallyControlled())
	{
		OnCooldownReplicatedDelegate.Broadcast(Index, CooldownValue);
	}
	else
	{
		ReplicateCooldownRpc(Index, CooldownValue);
	}
}

void USpellController::ReplicateCooldownRpc_Implementation(uint8 Index, float CooldownValue)
{
	if (Index < 0 || Index >= RepCooldowns.Num()) return;

	RepCooldowns[Index] = CooldownValue;

	OnCooldownReplicatedDelegate.Broadcast(Index, CooldownValue);
}

void USpellController::ResetCooldownsRpc_Implementation()
{
	for(int i = 0; i < RepCooldowns.Num(); i++)
	{
		RepCooldowns[i] = 0.0f;

		OnCooldownReplicatedDelegate.Broadcast(i, 0.0f);
	}
}

void USpellController::OnGlobalCooldownReplicated()
{
	OnGlobalCooldownChanged.Broadcast(bIsInGlobalCooldown);
}

void USpellController::UpdateSpellAimers()
{
	UpdateAttachSocket();
	
	for(int i = 0; i < SpellDatas.Num(); i++)
	{
		if (i >= SpellAimers.Num())
		{
			SpellAimers.Add(nullptr);
		}

		if (!SpellDatas[i])
		{
			if (SpellAimers[i])
			{
				SpellAimers[i]->Destroy();
				SpellAimers[i] = nullptr;
			}
			
			continue;
		}

		const auto Data = SpellDatas[i];
		
		if (SpellAimers[i] && SpellAimers[i]->GetClass() != Data->Aimer)
		{
			SpellAimers[i]->Destroy();
			SpellAimers[i] = nullptr;
		}

		if (!SpellAimers[i])
		{
			SpellAimers[i] = GetWorld()->SpawnActor<ASpellAimer>(Data->Aimer);
		}
		
		SpellAimers[i]->Init(this, Data->AimerParams, AttachSocket);
	}
}

void USpellController::SetupInputs()
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PlayerController) return;

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!InputSubsystem) return;

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		for(int i = 0; i < SpellInputs.Num(); i++)
		{
			const auto Input = SpellInputs[i];
			
			EnhancedInputComponent->BindAction(Input, ETriggerEvent::Started, this, &USpellController::OnSpellInputStarted, i);

			// Commented -> Normal Cast , Uncommented -> SmartCast
			//EnhancedInputComponent->BindAction(Input, ETriggerEvent::Completed, this, &USpellController::OnSpellInputStopped, i);

			//EnhancedInput de l'auto est a 0 et en modifier pulse
			if(i == 0)
				EnhancedInputComponent->BindAction(Input, ETriggerEvent::Triggered, this, &USpellController::OnSpellInputStarted, i );

		}

		EnhancedInputComponent->BindAction(ValidateInput, ETriggerEvent::Started, this, &USpellController::OnValidateInputStarted);
		EnhancedInputComponent->BindAction(CancelInput, ETriggerEvent::Started, this, &USpellController::OnCancelInputStarted);
	}
}

void USpellController::OnSpellInputStarted(const int Index)
{
	if (IsInCooldown(Index)) return;

	ASpellAimer* Aimer = nullptr;
	if (!TryGetSpellAimer(Index, Aimer)) return;
	
	if (!CanStartAiming(Index)) return;

	const auto Spell = GetSpellData(Index);
	
	if (Spell->IsInstantAiming)
	{
		Aimer->ForceUpdateAimResult();
		StopAndCast(Aimer, Index);
		return;
	}

	Aimer->Start();
	ActiveAimer = Aimer;
}

void USpellController::OnSpellInputStopped(int Index)
{
	ASpellAimer* Aimer = nullptr;
	if (!TryGetSpellAimer(Index, Aimer)) return;

	if (!Aimer->bIsAiming) return;

	StopAndCast(Aimer, Index);
}

void USpellController::OnValidateInputStarted()
{
	if (!IsValid(ActiveAimer)) return;

	const auto AimerIndex = SpellAimers.IndexOfByKey(ActiveAimer);

	StopAndCast(ActiveAimer, AimerIndex);
}

void USpellController::StopAndCast(ASpellAimer* Aimer, const int Index)
{
	Aimer->Stop();
	ActiveAimer = nullptr;

	if (!Aimer->IsValid()) return;
	
	const auto Result = Aimer->GetAimResult();
	if (!Result)
	{
		UE_LOG(LogTemp, Error, TEXT("Aimer at index %d of class %s gave no result. Spell will not be casted."), Index, *Aimer->GetClass()->GetName());
		return;
	}
	
	const auto Validity = ASpellManager::GetSpellRequestValidity(Index, this, GetWorld()->GetTimeSeconds());

	if (Validity == ESpellRequestValidity::Invalid)
	{
		return;
	}

	// Host doesn't need prediciton
	if (!UKismetSystemLibrary::IsServer(this))
	{
		CastState->Spell = GetSpellData(Index);
		CastState->SpellIndex = Index;
		CastState->IsCasting = true;
	
		OnPredictedCast(Index, Result);
	}

	RequestSpellCastGenericResultToServer(Index, Result);
}

void USpellController::OnCancelInputStarted()
{
	if (!IsValid(ActiveAimer)) return;

	ActiveAimer->Stop();
	ActiveAimer = nullptr;
}

void USpellController::StartGlobalCooldown()
{
	GlobalCooldown = GlobalCooldownValue;
	ReplicateGlobalCooldown(true);
}

void USpellController::StartCooldown(const int Index)
{
	if (Index < 0 || Index >= Cooldowns.Num()) return;

	const auto Value = GetSpellData(Index)->Cooldown;
	
	Cooldowns[Index] = Value;
	RepCooldowns[Index] = Value;

	SrvReplicateCooldown(Index, Value);
}

void USpellController::SrvResetAllCooldowns()
{
	const auto IsLocallyControlled = IsOwnerLocallyControlled();

	for(int i = 0; i < Cooldowns.Num(); i++)
	{
		Cooldowns[i] = 0.0f;
		RepCooldowns[i] = 0.0f;

		if(IsLocallyControlled)
		{
			OnCooldownReplicatedDelegate.Broadcast(i, 0.0f);
		}
	}

	if (!IsLocallyControlled)
	{
		ResetCooldownsRpc();
	}
}

void USpellController::UpdateCooldowns(float DeltaTime)
{
	if (GlobalCooldown > 0.0f)
	{
		GlobalCooldown -= DeltaTime;
		if (GlobalCooldown <= 0.0f)
		{
			ReplicateGlobalCooldown(false);
		}
	}
	
	for (int i = 0; i < Cooldowns.Num(); i++)
	{
		if (Cooldowns[i] <= 0.0f) continue;

		Cooldowns[i] -= DeltaTime;
		
		const auto NewValue = Cooldowns[i];
		if (RepCooldowns[i] - NewValue >= CooldownsReplicationDelay || NewValue <= 0)
		{
			RepCooldowns[i] = NewValue;

			SrvReplicateCooldown(i, NewValue);
		}
	}
}

bool USpellController::IsInCooldown(const int Index) const
{
	if (bIsInGlobalCooldown)
	{
		return true;
	}

	return RepCooldowns[Index] > 0;
}

bool USpellController::CanStartAiming(const int SpellIndex) const
{
	if (!CanStartAiming_BP(SpellIndex)) return false;
	
	if (IsCasting())
	{
		if (!CanCombo(SpellIndex)) return false;
	}
	
	return !IsAiming();
}

bool USpellController::TryGetSpellAimer(int Index, ASpellAimer*& OutAimer) const
{
	if (Index < 0 || Index >= SpellAimers.Num()) return false;

	OutAimer = SpellAimers[Index];

	return IsValid(OutAimer);
}

bool USpellController::IsOwnerLocallyControlled() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(Owner->GetOwner()))
	{
		return PlayerController->IsLocalController();
	}

	return false;
}

void USpellController::UpdateAttachSocket()
{
	if (AttachSocket) return;
	
	auto Components = GetOwner()->GetComponentsByTag(UArrowComponent::StaticClass(), "SpellSocket");

	if (Components.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No SpellSocket found"));
		return;
	}

	AttachSocket = Cast<USceneComponent>(Components[0]);
}

void USpellController::TrySelectSpellRpc_Implementation(const int Index, USpellDataAsset* Spell)
{
	SrvSelectSpell(Index, Spell);
}

void USpellController::SrvSelectSpell(const int Index, USpellDataAsset* Spell, const bool bShouldReplicate)
{
	UE_LOG(LogTemp, Warning, TEXT("SrvSelectSpell called with index %d and spell %s"), Index, *Spell->GetName());
	if (Index < 0 || Index >= SpellDatas.Num()) return;

	SpellDatas[Index] = Spell;

	if (bShouldReplicate)
	{
		ReplicateSpellDatas(SpellDatas);
	}
}

void USpellController::RequestSpellCastGenericResultToServer(const int SpellIndex, UAimResultHolder* Result)
{
	const auto ResultClass = Result->GetClass();
	
	// Can't switch. Only works on ints
	if (ResultClass == UVectorAimResultHolder::StaticClass())
	{
		const auto VectorResult = Cast<UVectorAimResultHolder>(Result);
		RequestSpellCastFromControllerRpc_Vector(SpellIndex, this, VectorResult->Vector);
	}
	else if(ResultClass == UActorAimResultHolder::StaticClass())
	{
		const auto ActorResult = Cast<UActorAimResultHolder>(Result);

		RequestSpellCastFromControllerRpc_Actor(SpellIndex, this, ActorResult->Actor);
	}
}

void USpellController::RequestSpellCastFromControllerRpc_Actor_Implementation(int SpellIndex, USpellController* Caster,
                                                                              AActor* Result)
{
	const auto Holder = NewObject<UActorAimResultHolder>();
	Holder->Actor = Result;

	const auto InSceneManagers = GetWorld()->GetGameInstance()->GetSubsystem<UInSceneManagersRefs>();
	const auto SpellManager = InSceneManagers->GetManager<ASpellManager>();
	
	SpellManager->RequestSpellCastFromController(SpellIndex, Caster, Holder);
}

void USpellController::RequestSpellCastFromControllerRpc_Vector_Implementation(const int SpellIndex, USpellController* Caster,
                                                                               const FVector Result)
{
	const auto Holder = NewObject<UVectorAimResultHolder>();
	Holder->Vector = Result;

	const auto InSceneManagers = GetWorld()->GetGameInstance()->GetSubsystem<UInSceneManagersRefs>();
	const auto SpellManager = InSceneManagers->GetManager<ASpellManager>();
	
	SpellManager->RequestSpellCastFromController(SpellIndex, Caster, Holder);
}

void USpellController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(USpellController, bIsInGlobalCooldown, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(USpellController, SpellDatas, COND_OwnerOnly);
}
