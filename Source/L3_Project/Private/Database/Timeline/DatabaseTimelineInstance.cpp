#include "Database/Timeline/DatabaseTimelineInstance.h"

#include "Database/DatabaseFunctions.h"
#include "Effects/Effectable.h"
#include "Effects/EffectDataAsset.h"
#include "Effects/EffectType.h"
#include "Misc/Guid.h"
#include "Components/ActorComponent.h"
#include "Networking/ZodiaqCharacter.h"
#include "Networking/ZodiaqPlayerController.h"
#include "Networking/ZodiaqPlayerState.h"

TArray<FDynamicCombatEvent> UDatabaseTimelineInstance::Events;
FString UDatabaseTimelineInstance::MatchId;

void UDatabaseTimelineInstance::Init()
{
	Events.Empty();
	MatchId = GenerateRandomMatchId();

	UEffectable::SrvOnEffectAddedDelegate.AddStatic(OnEffectAdded);
}

void UDatabaseTimelineInstance::AddEvent(const FString& Type, const int32 Timestamp, const TSharedPtr<FJsonObject>& Data)
{
	FDynamicCombatEvent NewEvent;
	NewEvent.Timestamp = Timestamp;
	NewEvent.Type = Type;
	NewEvent.Data = Data;

	Events.Add(NewEvent);
}

void UDatabaseTimelineInstance::UploadTimeline(const FString& PlayerIdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
	const FString Path = FString::Printf(TEXT("GAMES/%s"), *MatchId);
	TSharedPtr<FJsonObject> Payload = ConvertToJson();

	UDatabaseFunctions::SetData(Path, Payload, PlayerIdToken, OnSuccess, OnFailure);
}

TSharedPtr<FJsonObject> UDatabaseTimelineInstance::ConvertToJson()
{
	TSharedPtr<FJsonObject> Root = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> JsonArray;

	for (const FDynamicCombatEvent& Event : Events)
	{
		const TSharedPtr<FJsonObject> EventJson = MakeShareable(new FJsonObject);

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

void UDatabaseTimelineInstance::OnEffectAdded(UEffectable* effectable, UEffectDataAsset* effect, AActor* actor, FGuid guid) 
{
	//TODO c'est pas encore fini
	const TSharedPtr<FJsonObject> EffectJson = MakeShareable(new FJsonObject);

	AZodiaqCharacter* CharacterTarget = Cast<AZodiaqCharacter>(effectable->GetOwner());
	if (CharacterTarget != nullptr) //Player
	{
		const FString Id = Cast<AZodiaqPlayerController>(effectable->GetOwner())->GetPlayerState<AZodiaqPlayerState>()->ClientData.UUID;
	
		EffectJson->SetStringField(TEXT("Target"), Id);
	}
	else //BOSS
	{
		EffectJson->SetStringField(TEXT("Target"), "Boss"); //Not beau but osef
	}

	AZodiaqCharacter* CharacterCaster = Cast<AZodiaqCharacter>(actor->GetOwner());
	if (CharacterCaster != nullptr) //Player
	{
		const FString Id = Cast<AZodiaqPlayerController>(actor->GetOwner())->GetPlayerState<AZodiaqPlayerState>()->ClientData.UUID;
	
		EffectJson->SetStringField(TEXT("Caster"), Id);
	}
	else //BOSS
	{
		EffectJson->SetStringField(TEXT("Caster"), "Boss"); //Not beau but osef
	}
	
	switch (effect->Type)
	{
	case EEffectType::Attack:
		EffectJson->SetStringField(TEXT("Type"), TEXT("Attack"));
		break;
	case EEffectType::Defense:
		EffectJson->SetStringField(TEXT("Type"), TEXT("Defense"));
		break;
	case EEffectType::MoveSpeed:
		EffectJson->SetStringField(TEXT("Type"), TEXT("MoveSpeed"));
		break;
	case EEffectType::HealBonus:
		EffectJson->SetStringField(TEXT("Type"), TEXT("HealBonus"));
		break;
	case EEffectType::Stun:
		EffectJson->SetStringField(TEXT("Type"), TEXT("Stun"));
		break;
	case EEffectType::Root:
		EffectJson->SetStringField(TEXT("Type"), TEXT("Root"));
		break;
	case EEffectType::DamageOverTime:
		EffectJson->SetStringField(TEXT("Type"), TEXT("DamageOverTime"));
		break;
	case EEffectType::HealOverTime:
		EffectJson->SetStringField(TEXT("Type"), TEXT("HealOverTime"));
		break;
	}
	EffectJson->SetNumberField(TEXT("Value"), effect->GetValue(EEffectValueType::Value));
	
	AddEvent("Effect",GetGameTimeSecondsStatic(effect) , EffectJson);
}

float UDatabaseTimelineInstance::GetGameTimeSecondsStatic(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return 0.0f;

	const UWorld* World = WorldContextObject->GetWorld();
	return World ? World->GetTimeSeconds() : 0.0f;
}
