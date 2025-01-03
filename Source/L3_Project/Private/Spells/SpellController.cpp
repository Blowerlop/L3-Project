// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/SpellController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InSceneManagersRefs.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Spells/SpellAimer.h"
#include "Spells/SpellDataAsset.h"
#include "Spells/SpellManager.h"
#include "Spells/Results/VectorAimResultHolder.h"

USpellController::USpellController()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
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

	if (!UKismetSystemLibrary::IsServer(this)) return;
	
	Cooldowns.SetNum(Max_Spells);
	
	SpellDatas.SetNum(Max_Spells);
	RepCooldowns.SetNum(Max_Spells);

	ReplicateSpellDatas(SpellDatas);
	ReplicateCooldowns(RepCooldowns);
}

void USpellController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!UKismetSystemLibrary::IsServer(this)) return;
	
	UpdateCooldowns(DeltaTime);
}

void USpellController::ReplicateCooldowns(const TArray<int>& NewCooldowns)
{
	RepCooldowns = NewCooldowns;

	// For some reasons C++ RepUsing doesn't trigger replication method on listen server, but blueprint RepNotify does
	if (IsNetMode(NM_ListenServer) && IsOwnerLocallyControlled())
	{
		OnCooldownsReplicated();
	}
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

void USpellController::OnCooldownsReplicated()
{
}

void USpellController::OnGlobalCooldownReplicated()
{
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

		if (SpellAimers[i] && SpellAimers[i]->GetClass() != SpellDatas[i]->Aimer)
		{
			SpellAimers[i]->Destroy();
			SpellAimers[i] = nullptr;
		}

		if (!SpellAimers[i])
		{
			SpellAimers[i] = GetWorld()->SpawnActor<ASpellAimer>(SpellDatas[i]->Aimer);
		}
		
		SpellAimers[i]->Init(AttachSocket);
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
			EnhancedInputComponent->BindAction(Input, ETriggerEvent::Completed, this, &USpellController::OnSpellInputStopped, i);
		}
	}
}

void USpellController::OnSpellInputStarted(const int Index)
{
	if (IsInCooldown(Index)) return;

	ASpellAimer* Aimer = nullptr;
	if (!TryGetSpellAimer(Index, Aimer)) return;
	
	if (!CanStartAiming()) return;

	Aimer->Start();
}

void USpellController::OnSpellInputStopped(int Index)
{
	ASpellAimer* Aimer = nullptr;
	if (!TryGetSpellAimer(Index, Aimer)) return;

	if (!Aimer->bIsAiming) return;

	Aimer->Stop();

	TryCastSpellGenericResultToServer(Index, GetOwner(), Aimer->GetAimResult());
}

void USpellController::StartGlobalCooldown()
{
	GlobalCooldown = GlobalCooldownValue;
	ReplicateGlobalCooldown(true);
}

void USpellController::StartCooldown(const int Index)
{
	if (Index < 0 || Index >= Cooldowns.Num()) return;

	Cooldowns[Index] = GetSpellData(Index)->Cooldown;
	RepCooldowns[Index] = FMath::CeilToInt(Cooldowns[Index]);
	
	ReplicateCooldowns(RepCooldowns);
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
	
	bool bCooldownsChanged = false;
	
	for (int i = 0; i < Cooldowns.Num(); i++)
	{
		if (Cooldowns[i] <= 0.0f) continue;
		
		const auto RepCooldown = RepCooldowns[i];
		Cooldowns[i] -= DeltaTime;

		const auto NewValue = FMath::CeilToInt(Cooldowns[i]);

		if (NewValue != RepCooldown)
		{
			RepCooldowns[i] = NewValue;
			bCooldownsChanged = true;
		}
	}

	if (bCooldownsChanged)
	{
		ReplicateCooldowns(RepCooldowns);
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

bool USpellController::CanStartAiming() const
{
	return !SpellAimers.ContainsByPredicate([](const ASpellAimer* Aimer) { return Aimer && Aimer->bIsAiming; });
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
	if (Index < 0 || Index >= SpellDatas.Num()) return;

	SpellDatas[Index] = Spell;

	ReplicateSpellDatas(SpellDatas);
}

void USpellController::TryCastSpellGenericResultToServer(const int SpellIndex, AActor* Caster, UAimResultHolder* Result) const
{
	const auto ResultClass = Result->GetClass();
	
	// Can't switch. Only works on ints
	if (ResultClass == UVectorAimResultHolder::StaticClass())
	{
		const auto VectorResult = Cast<UVectorAimResultHolder>(Result);
		TryCastSpellFromControllerRpc_Vector(SpellIndex, Caster, VectorResult->Vector);
	}
}

void USpellController::TryCastSpellFromControllerRpc_Vector_Implementation(const int SpellIndex, AActor* Caster, const FVector Result) const
{
	const auto Holder = NewObject<UVectorAimResultHolder>();
	Holder->Vector = Result;

	const auto InSceneManagers = GetWorld()->GetGameInstance()->GetSubsystem<UInSceneManagersRefs>();
	const auto SpellManager = InSceneManagers->GetManager<ASpellManager>();
	
	SpellManager->TryCastSpellFromController(SpellIndex, Caster, Holder);
}

void USpellController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(USpellController, RepCooldowns, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(USpellController, bIsInGlobalCooldown, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(USpellController, SpellDatas, COND_OwnerOnly);
}
