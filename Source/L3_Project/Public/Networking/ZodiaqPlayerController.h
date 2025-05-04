#pragma once
#include "ZodiaqPlayerController.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AZodiaqPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(Server, Reliable)
	void ServerRequestTime(double ClientTimestamp);

	UFUNCTION(Client, Reliable)
	void ClientReceiveTime(double ClientTimestamp, double ServerTime);

private:
	double LastSyncClientTime = 0.0;
	float SyncInterval = 5.f;
	float TimeSinceLastSync = 0.f;
};
