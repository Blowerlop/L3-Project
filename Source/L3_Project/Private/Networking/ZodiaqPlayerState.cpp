#include "Networking/ZodiaqPlayerState.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Networking/ZodiaqGameMode.h"

void AZodiaqPlayerState::LoadCharacter(const FSerializableCharacterData CharacterData)
{
	if (bIsCharacterLoaded) return;

	bIsCharacterLoaded = true;
	ClientData.CharacterData = CharacterData;
}

void AZodiaqPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!UKismetSystemLibrary::IsServer(this)) return;

	const auto GameMode = GetWorld()->GetAuthGameMode();
	if (!IsValid(GameMode)) return;

	const auto ZodiaqGameMode = Cast<AZodiaqGameMode>(GameMode);
	if (!IsValid(ZodiaqGameMode)) return;

	ZodiaqGameMode->OnPlayerStateEndPlay(this);
}
