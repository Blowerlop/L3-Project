#include "Networking/ZodiaqGameState.h"

#include "Kismet/KismetSystemLibrary.h"

AZodiaqGameState::AZodiaqGameState()
{
	// Disable default unreal network time sync
	ServerWorldTimeSecondsUpdateFrequency = 0;
	PrimaryActorTick.bCanEverTick = true;
}

double AZodiaqGameState::GetServerWorldTimeSeconds() const
{
	return FPlatformTime::Seconds() + TimeOffset + (UKismetSystemLibrary::IsServer(this) ? 5 : 0);
}

void AZodiaqGameState::SetServerTimeOffset(const double Offset)
{
	TimeOffset = Offset; //FMath::Lerp(TimeOffset, Offset, 0.5f);
}

void AZodiaqGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!UKismetSystemLibrary::IsServer(this))
	{
		return;
	}
	
	if (GEngine)
	{
		/*GEngine->AddOnScreenDebugMessage(
			5778676,                        // ID constant → remplace le message précédent
			0.0f,                     // 0 = affiche uniquement pour ce frame
			FColor::Cyan,            // Couleur du texte
			FString::Printf(TEXT("Estimated Server Time: %.3f"), GetServerWorldTimeSeconds())
		);*/
	}
}
