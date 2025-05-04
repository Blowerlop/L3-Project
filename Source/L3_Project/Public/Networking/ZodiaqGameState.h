#pragma once
#include "GameFramework/GameStateBase.h"
#include "ZodiaqGameState.generated.h"

UCLASS(Blueprintable, BlueprintType)
class AZodiaqGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AZodiaqGameState();
	
	virtual double GetServerWorldTimeSeconds() const override;

	void SetServerTimeOffset(double Offset);

	virtual void Tick(float DeltaSeconds) override;

private:
	double TimeOffset = 0.f;
};
