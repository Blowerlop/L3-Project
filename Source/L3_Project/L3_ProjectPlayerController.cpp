// Copyright Epic Games, Inc. All Rights Reserved.

#include "L3_ProjectPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerState.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AL3_ProjectPlayerController::AL3_ProjectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AL3_ProjectPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AL3_ProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AL3_ProjectPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AL3_ProjectPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AL3_ProjectPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AL3_ProjectPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AL3_ProjectPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AL3_ProjectPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AL3_ProjectPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AL3_ProjectPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AL3_ProjectPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AL3_ProjectPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AL3_ProjectPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AL3_ProjectPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AL3_ProjectPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT

bool AL3_ProjectPlayerController::bShowNetworkInfos = false;
float AL3_ProjectPlayerController::NetworkInfosTimer = 0.0f;

void AL3_ProjectPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bShowNetworkInfos)
	{
		NetworkInfosTimer += DeltaSeconds;

		if(NetworkInfosTimer > 0.5f)
		{
			if (GEngine)
			{
				auto Ping = 0.0f;

				if (GetNetMode() != NM_ListenServer && PlayerState)
				{
					Ping = PlayerState->GetPingInMilliseconds();
				}
				
				GEngine->AddOnScreenDebugMessage(1001, 0.5f, FColor::Green,
					FString::Printf(TEXT("Ping: %f"), Ping));

				if(const auto Connection = GetNetConnection())
				{
					if (Connection->GetDriver() && Connection->GetDriver()->GetClass())
					{
						GEngine->AddOnScreenDebugMessage(1002, 0.5f, FColor::Green, FString::Printf(TEXT("NetDriver: %s"),
							*Connection->GetDriver()->GetClass()->GetName()));
					}

					GEngine->AddOnScreenDebugMessage(1003, 0.5f, FColor::Green, FString::Printf(TEXT("In Loss Percentage: %f"),
						Connection->GetInLossPercentage().GetLossPercentage()));

					GEngine->AddOnScreenDebugMessage(1004, 0.5f, FColor::Green, FString::Printf(TEXT("Out Loss Percentage: %f"),
						Connection->GetOutLossPercentage().GetLossPercentage()));
				}
			}
			
			NetworkInfosTimer = 0.0f;
		}
	}
}

void AL3_ProjectPlayerController::ShowNetworkInfos()
{
	bShowNetworkInfos = !bShowNetworkInfos;
}

#endif
