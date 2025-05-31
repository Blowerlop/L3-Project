// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/BaseGameInstance.h"

#include "OnlineSubsystemUtils.h"
#include "Database/DatabaseFunctions.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Networking/InstancesManagerSubsystem.h"
#include "Networking/SessionsManagerSubsystem.h"

const FString UBaseGameInstance::UUIDConnectOptionsKey = "UUID";
const FString UBaseGameInstance::UserNameConnectOptionsKey = "UserName";
const FString UBaseGameInstance::CharacterUUIDConnectOptionsKey = "CUUID";
FString UBaseGameInstance::FirebaseIdToken = TEXT("");

#pragma region Login/out

void UBaseGameInstance::Login(const bool bUseDevTool, FString AuthToolId)
{
	if (const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld(), "EOS"))
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

			LoginDelegateHandle = IdentityInterface->OnLoginCompleteDelegates->AddUObject(this, &UBaseGameInstance::OnLoginComplete);
			IdentityInterface->Login(0, Credentials);
		}
	}
}

void UBaseGameInstance::Logout() const
{
	if (const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld(), "EOS"))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface(); IdentityInterface.IsValid())
		{
			IdentityInterface->Logout(0);
		}
	}
}

bool UBaseGameInstance::IsLoggedIn() const
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), "EOS");
	const IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();

	return IdentityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}

#pragma endregion 

#pragma region Lifecycle

void UBaseGameInstance::Init()
{
	Super::Init();

	FirebaseIdToken = TEXT("");
	
	if(GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UBaseGameInstance::OnNetworkFailure);
	}

	if (IsRunningDedicatedServer())
	{
		FString Email{};
		FString Password{};
		if (!UDatabaseFunctions::LoadFirebaseAdminConfig(Email, Password))
		{
#if WITH_EDITOR
			UE_LOG(LogTemp, Error, TEXT("Failed to load Firebase Admin config from Keys.json. Server will not run correctly."));
#else
			UE_LOG(LogTemp, Fatal, TEXT("Failed to load Firebase Admin config from Keys.json. Server will not run correctly."));
#endif
			return;
		}

		FSuccess SuccessCallback{};
		SuccessCallback.BindUFunction(this, "OnServerAuthSuccess");
		
		FFailed FailedCallback{};
		FailedCallback.BindUFunction(this, "OnServerAuthFailure");
		
		UDatabaseFunctions::AuthRequest(Email, Password, SuccessCallback, FailedCallback);
	}
}

void UBaseGameInstance::OnServerAuthSuccess(const FString& Message)
{
	UE_LOG(LogTemp, Log, TEXT("Server auth success: %s"), *Message);
}

void UBaseGameInstance::OnServerAuthFailure(const FString& ErrorMessage)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Error, TEXT("Server auth failed: %s"), *ErrorMessage);
#else
	UE_LOG(LogTemp, Fatal, TEXT("Server auth failed: %s"), *ErrorMessage);
#endif
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

void UBaseGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Login successful! UserId: %s"), *UserId.ToString());

		//SelfClientData = FClientData(UserId.ToString(), UserName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Login failed: %s"), *Error);
	}

	if (const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld(), "EOS"))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface(); IdentityInterface.IsValid())
		{
			IdentityInterface->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginDelegateHandle);
			LoginDelegateHandle.Reset();
		}
	}
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

void UBaseGameInstance::OnFirebaseLogout()
{
	FirebaseIdToken = TEXT("");
	SelfClientData = FClientData();
}

#pragma endregion

#pragma region Debug

void UBaseGameInstance::DebugLoginWithDevAuthTool(const FString& Args)
{
	Login(true, Args);
}

void UBaseGameInstance::SetHostingType(const FString& Args)
{
	UInstancesManagerSubsystem::SetHostingType(Args);
}

void UBaseGameInstance::PrintServerInfos()
{
	if (GEngine)
	{
		const FWorldContext* WorldContext = Algo::FindByPredicate(GEngine->GetWorldContexts(), [](const FWorldContext& Context) {
			return Context.World() != nullptr;
		});
		
		if (WorldContext)
		{
			for(auto a : WorldContext->ActiveNetDrivers)
			{
				UE_LOG(LogTemp, Error, TEXT("ActiveNetDrivers: %s %s"), *a.NetDriver.GetClass()->GetName(), *FName(NAME_GameNetDriver).ToString());
				UE_LOG(LogTemp, Error, TEXT("aeffafaef: %s"), *FName(NAME_PendingNetDriver).ToString());
			}
			
			if (const auto World = WorldContext->World())
			{
				auto URL = World->URL;
				auto Address = URL.ToString();
				auto Port = URL.Port;

				UE_LOG(LogTemp, Error, TEXT("Server infos: %s:%d"), *Address, Port);

				if (auto NetDriver = World->GetNetDriver())
				{
					auto NetMode = NetDriver->GetNetMode();
					auto Role = NetDriver->RoleProperty->GetFullName();
					auto RemoteRole = NetDriver->RemoteRoleProperty->GetFullName();

					uint32 addr = 0;
					int32 port = 0;
					NetDriver->GetLocalAddr()->GetIp(addr);
					NetDriver->GetLocalAddr()->GetPort(port);
					
					UE_LOG(LogTemp, Error, TEXT("NetDriver: %s NetMode: %d, Role: %s, RemoteRole: %s"), *NetDriver->GetClass()->GetName(), NetMode, *Role, *RemoteRole);
					UE_LOG(LogTemp, Error, TEXT("LocalAddr: %d:%d"), addr, port);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find a valid world context"));
		}
	}
}

#pragma endregion 
