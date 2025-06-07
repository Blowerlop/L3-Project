#include "Database/Timeline/DatabaseTimelineInstance.h"
#include "Database/DatabaseFunctions.h"
#include "Misc/Guid.h"

void UDatabaseTimelineInstance::Init()
{
	Events.Empty();
	MatchId = GenerateRandomMatchId();
}

void UDatabaseTimelineInstance::AddEvent(const FString& Type, int32 Timestamp, const TSharedPtr<FJsonObject>& Data)
{
	FDynamicCombatEvent NewEvent;
	NewEvent.Timestamp = Timestamp;
	NewEvent.Type = Type;
	NewEvent.Data = Data;

	Events.Add(NewEvent);
}

void UDatabaseTimelineInstance::UploadTimeline(const FString& PlayerIdToken, const FSuccess& OnSuccess, const FFailed& OnFailure) const
{
	const FString Path = FString::Printf(TEXT("GAMES/%s"), *MatchId);
	TSharedPtr<FJsonObject> Payload = ConvertToJson();

	UDatabaseFunctions::SetData(Path, Payload, PlayerIdToken, OnSuccess, OnFailure);
}

TSharedPtr<FJsonObject> UDatabaseTimelineInstance::ConvertToJson() const
{
	TSharedPtr<FJsonObject> Root = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> JsonArray;

	for (const FDynamicCombatEvent& Event : Events)
	{
		TSharedPtr<FJsonObject> EventJson = MakeShareable(new FJsonObject);

		EventJson->SetNumberField(TEXT("timestamp"), Event.Timestamp);
		EventJson->SetStringField(TEXT("type"), Event.Type);
		EventJson->SetObjectField(TEXT("data"), Event.Data);

		JsonArray.Add(MakeShareable(new FJsonValueObject(EventJson)));
	}

	Root->SetArrayField("timeline", JsonArray);
	Root->SetStringField("match_id", MatchId); // I added it but idk if it's rlly usefull \_(°_°)_/
	return Root;
}

FString UDatabaseTimelineInstance::GenerateRandomMatchId()
{
	return FGuid::NewGuid().ToString(EGuidFormats::Digits); // Format court sans tirets
}
