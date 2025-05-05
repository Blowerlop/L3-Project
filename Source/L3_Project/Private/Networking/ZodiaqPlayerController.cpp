#include "Networking/ZodiaqPlayerController.h"

#include "Algo/RandomShuffle.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Networking/ZodiaqGameState.h"

int id;

void AZodiaqPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TimeSinceLastSync = SyncInterval;
	id = FMath::RandRange(0, 100000);
}

void AZodiaqPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (UKismetSystemLibrary::IsServer(this)) return;
	
	TimeSinceLastSync += DeltaSeconds;
	if (TimeSinceLastSync >= SyncInterval)
	{
		LastSyncClientTime = FPlatformTime::Seconds();
		//UE_LOG(LogTemp, Error, TEXT("Requesting server time... Client time: %f"), LastSyncClientTime);
		ServerRequestTime(LastSyncClientTime);
		TimeSinceLastSync = 0.f;
	}
	
	if (GEngine)
	{
		if(!GetWorld()->GetGameState()) return;
		
		GEngine->AddOnScreenDebugMessage(
			id,                        // ID constant → remplace le message précédent
			0.0f,                     // 0 = affiche uniquement pour ce frame
			FColor::Cyan,            // Couleur du texte
			FString::Printf(TEXT("Estimated Server Time: %.3f %s"),  GetWorld()->GetGameState()->GetServerWorldTimeSeconds(), *GetName())
		);
	}
}

void AZodiaqPlayerController::ServerRequestTime_Implementation(double ClientTimestamp)
{
	const auto ServerTime = FPlatformTime::Seconds() + 5; //;
	//UE_LOG(LogTemp, Error, TEXT("Server received time request... Server time: %f"), ServerTime);
	ClientReceiveTime(ClientTimestamp, ServerTime);
}

void AZodiaqPlayerController::ClientReceiveTime_Implementation(double ClientTimestamp, double ServerTime)
{
	const auto Now = FPlatformTime::Seconds(); //FPlatformTime::Seconds();
	const auto RoundTrip = Now - ClientTimestamp;
	const auto EstimatedOneWay = RoundTrip * 0.5;

	const auto NewOffset = ServerTime + EstimatedOneWay - Now;

	//UE_LOG(LogTemp, Error, TEXT("Client received time... Client timestamp: %f, Now: %f, Server time: %f, Round trip: %f, Estimated one way: %f, New offset: %f"), 
		//ClientTimestamp, Now, ServerTime, RoundTrip, EstimatedOneWay, NewOffset);
	
	const auto GameState = GetWorld()->GetGameState<AZodiaqGameState>();
	if (IsValid(GameState))
	{
		GameState->SetServerTimeOffset(NewOffset);
	}
}
