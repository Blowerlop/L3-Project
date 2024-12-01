// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/BaseGameInstance.h"

#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Networking/InstancesManagerSubsystem.h"
#include "Networking/SessionsManagerSubsystem.h"

#pragma region Login/out

void UBaseGameInstance::Login(const bool bUseDevTool, FString AuthToolId) const
{
	if (const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface(); IdentityInterface.IsValid())
		{
			FOnlineAccountCredentials Credentials;

			if (bUseDevTool)
			{
				Credentials.Type = TEXT("developer");
				Credentials.Id = "localhost:8080";
				Credentials.Token = AuthToolId;
			}
			else
			{
				Credentials.Type = TEXT("deviceID");
				Credentials.Id = "";
				Credentials.Token = "";
			}
				
			IdentityInterface->Login(0, Credentials);
		}
	}
}

void UBaseGameInstance::Logout() const
{
	if (const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface(); IdentityInterface.IsValid())
		{
			IdentityInterface->Logout(0);
		}
	}
}

bool UBaseGameInstance::IsLoggedIn() const
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();

	return IdentityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}

#pragma endregion 

#pragma region Lifecycle

void UBaseGameInstance::Init()
{
	Super::Init();

	if(GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UBaseGameInstance::OnNetworkFailure);
	}
}

void UBaseGameInstance::Shutdown()
{
	Super::Shutdown();

	// Can't be done in subsystem Deinitialize because it should be done before Logout.
	if (const auto Subsystem = GetSubsystem<USessionsManagerSubsystem>())
	{
		if (Subsystem->HasRunningSession)
		{
			Subsystem->DestroySession();
		}
	}

	if (!IsRunningDedicatedServer())
	{
		Logout();
	}
}

void UBaseGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg,
	const FString& String)
{
	UE_LOG(LogTemp, Error, TEXT("Network failure. %s %d"), *String, Arg);

	// Try to return to lobby if in instance
	// If already in lobby, or can't return to lobby, go back to main menu
}

#pragma endregion

#pragma region Transition

void UBaseGameInstance::StartTransition(ENetTransitionType TransitionType)
{
	auto Map = TEXT("");
	
	switch (TransitionType)
	{
		case ENetTransitionType::InstanceToLobby:
		case ENetTransitionType::LobbyToInstance:
			Map = TEXT("/Game/_Project/000-Game/Maps/TransitionMap");
			break;
		default:
			break;
	}
	
	if (APlayerController* PlayerController = GetFirstLocalPlayerController())
	{
		PlayerController->ClientTravel(Map, TRAVEL_Absolute);
	}
}

void UBaseGameInstance::StartTransition(const ENetTransitionType TransitionType, const TransitionFunc& Func)
{
	TransitionDelegate.BindLambda(Func);
	StartTransition(TransitionType);
}

void UBaseGameInstance::OnTransitionEntered()
{
	(void)TransitionDelegate.ExecuteIfBound();
	TransitionDelegate.Unbind();
}

#pragma endregion

#pragma region Debug

void UBaseGameInstance::DebugLoginWithDevAuthTool(const FString& Args) const
{
	Login(true, Args);
}

void UBaseGameInstance::SetHostingType(const FString& Args)
{
	UInstancesManagerSubsystem::SetHostingType(Args);
}

#pragma endregion 
