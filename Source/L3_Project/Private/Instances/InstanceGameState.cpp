#include "Instances/InstanceGameState.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Networking/BaseGameInstance.h"
#include "Networking/InstancesManagerSubsystem.h"

int AInstanceGameState::GetConnectingOrConnectedPlayersCount() const
{
	int Count = 0;
	for (const auto& Player : InstancePlayers)
	{
		if (Player.State == EInstancePlayerState::Connecting || Player.State == EInstancePlayerState::Connected)
		{
			Count++;
		}
	}

	return Count;
}

void AInstanceGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!UKismetSystemLibrary::IsServer(this)) return;

	const auto GameInstance = GetGameInstance<UBaseGameInstance>();
	if (!IsValid(GameInstance)) return;

	const auto InstancesManager = GameInstance->GetSubsystem<UInstancesManagerSubsystem>();

	for (auto& Player : InstancesManager->CurrentInstanceSettings.Players)
	{
		const int InstanceId = FMath::RandRange(-9999999, 9999999);
		
		InstancePlayers.Add(FInstancePlayerData(InstanceId, Player.Name, EInstancePlayerState::Connecting));
		ClientUUIDToInstanceId.Add(Player.UUID, InstanceId);
	}

	ReplicatePlayersState();

	GameInstance->OnCharacterLoaded.AddDynamic(this, &AInstanceGameState::OnCharacterLoaded);
	GameInstance->OnCharacterDestroyed.AddDynamic(this, &AInstanceGameState::OnCharacterDestroyed);
}

void AInstanceGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	const auto GameInstance = GetGameInstance<UBaseGameInstance>();
	if (!IsValid(GameInstance)) return;
	
	GameInstance->OnCharacterLoaded.RemoveDynamic(this, &AInstanceGameState::OnCharacterLoaded);
	GameInstance->OnCharacterDestroyed.RemoveDynamic(this, &AInstanceGameState::OnCharacterDestroyed);
}

void AInstanceGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!UKismetSystemLibrary::IsServer(this)) return;
	if (TimeOutTimer >= TimeOutDuration) return;
	
	TimeOutTimer += DeltaSeconds;

	if (TimeOutTimer >= TimeOutDuration)
	{
		for (auto& Player : InstancePlayers)
		{
			if (Player.State == EInstancePlayerState::Connecting)
			{
				Player.State = EInstancePlayerState::Timeout;
			}
		}

		ReplicatePlayersState();
	}
}

void AInstanceGameState::OnRep_InstancePlayers()
{
	UE_LOG(LogTemp, Error, TEXT("Instance players replicated: %d"), InstancePlayers.Num());

	OnPlayersStateChanged.Broadcast(InstancePlayers);
}

void AInstanceGameState::OnCharacterLoaded(AZodiaqCharacter* Character)
{
	const auto ClientData = Character->GetClientData();

	if (!ClientUUIDToInstanceId.Contains(ClientData.UUID))
	{
		UE_LOG(LogTemp, Error, TEXT("Client with UUID %s joined but was not expected. Something went wrong."),
			*ClientData.UUID);
		return;
	}

	const auto InstanceId = ClientUUIDToInstanceId[ClientData.UUID];

	const auto Index = InstancePlayers.IndexOfByPredicate([InstanceId](const FInstancePlayerData& PlayerData) {
		return PlayerData.InstanceId == InstanceId;
	});

	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("OnCharacterSpawned: Client with UUID %s linked to InstanceId %d can't be found. Something went wrong."),
			*ClientData.UUID, InstanceId);
		return;
	}

	auto& InstancePlayer = InstancePlayers[Index];
	
	InstancePlayer.State = EInstancePlayerState::Connected;
	InstancePlayer.Character = Character;

	Character->InstanceId = InstanceId;

	ReplicatePlayersState();

	if (GetNetMode() == NM_ListenServer)
	{
		Character->OnRep_InstanceId();
	}
}

void AInstanceGameState::OnCharacterDestroyed(AZodiaqCharacter* Character)
{
	const auto InstanceId = Character->InstanceId;
	
	const auto Index = InstancePlayers.IndexOfByPredicate([InstanceId](const FInstancePlayerData& PlayerData) {
		return PlayerData.InstanceId == InstanceId;
	});

	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("OnCharacterDestroyed: Client with InstanceId %d not found. Something went wrong."),
			InstanceId);
		return;
	}

	auto& InstancePlayer = InstancePlayers[Index];
	InstancePlayer.State = EInstancePlayerState::Disconnected;
	InstancePlayer.Character = nullptr;
	
	ReplicatePlayersState();
}

void AInstanceGameState::ReplicatePlayersState()
{
	InstancePlayers = InstancePlayers; 
	
	if (GetNetMode() == NM_ListenServer)
	{
		OnRep_InstancePlayers();
	}
}

// Replicate props
void AInstanceGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInstanceGameState, InstancePlayers);
}
