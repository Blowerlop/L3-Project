#pragma once
#include "GameFramework/GameState.h"
#include "Networking/ZodiaqCharacter.h"
#include "Networking/ZodiaqGameState.h"
#include "InstanceGameState.generated.h"

UENUM(BlueprintType)
enum class EInstancePlayerState : uint8
{
	Connecting,
	Connected,
	Timeout,
	Disconnected
};

USTRUCT(BlueprintType)
struct FInstancePlayerData
{
	GENERATED_BODY()

	FInstancePlayerData() = default;
	
	explicit FInstancePlayerData(const int InInstanceId, const FString& InName, const EInstancePlayerState InState = EInstancePlayerState::Connecting)
		: InstanceId(InInstanceId), Name(InName), State(InState) {}
	
	UPROPERTY(BlueprintReadOnly)
	int InstanceId{};

	UPROPERTY(BlueprintReadOnly)
	FString Name{};

	UPROPERTY(BlueprintReadOnly)
	EInstancePlayerState State{};

	UPROPERTY(BlueprintReadOnly)
	AZodiaqCharacter* Character = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInstancePlayersStateChanged, const TArray<FInstancePlayerData>&, Players);

UCLASS(BlueprintType, Blueprintable)
class AInstanceGameState : public AZodiaqGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnInstancePlayersStateChanged OnPlayersStateChanged;

	UFUNCTION(BlueprintCallable)
	int GetConnectingOrConnectedPlayersCount() const;
	
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_InstancePlayers)
	TArray<FInstancePlayerData> InstancePlayers;

	constexpr static float TimeOutDuration = 30.0f;
	
	float TimeOutTimer;
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TMap<FString, int> ClientUUIDToInstanceId;
	
	UFUNCTION()
	void OnRep_InstancePlayers();

	UFUNCTION()
	void OnCharacterLoaded(AZodiaqCharacter* Character);
	
	UFUNCTION()
	void OnCharacterDestroyed(AZodiaqCharacter* Character);

	void ReplicatePlayersState();
};
