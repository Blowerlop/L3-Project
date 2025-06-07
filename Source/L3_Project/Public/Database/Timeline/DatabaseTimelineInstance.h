#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"

DECLARE_DELEGATE(FSuccess);
DECLARE_DELEGATE_OneParam(FFailed, const FString&);

USTRUCT()
struct FDynamicCombatEvent
{
	GENERATED_BODY()

	int32 Timestamp;
	FString Type;
	TSharedPtr<FJsonObject> Data;

	FDynamicCombatEvent() : Timestamp(0), Data(MakeShareable(new FJsonObject)) {}
};

UCLASS()
class L3_PROJECT_API UDatabaseTimelineInstance : public UObject
{
	GENERATED_BODY()

public:
	void Init();

	/**
	 * Add event to timeline
	 * @param Type - Event Type
	 * @param Timestamp - Event time                                                                                                                                                                       What a useless comment
	 * @param Data - Event JSon Data
	 */
	void AddEvent(const FString& Type, int32 Timestamp, const TSharedPtr<FJsonObject>& Data);

	/**
	 * Send data to DB (HOST ONLY) And should be done at the end of the game for pity
	 */
	void UploadTimeline(const FString& PlayerIdToken, const FSuccess& OnSuccess, const FFailed& OnFailure) const;

private:
	TArray<FDynamicCombatEvent> Events;
	FString MatchId;

	TSharedPtr<FJsonObject> ConvertToJson() const;

	static FString GenerateRandomMatchId();
};
