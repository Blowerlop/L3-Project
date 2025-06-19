#include "Database/Timeline/DatabaseTimelineInstance.h"

#include <string>

#include "Database/DatabaseFunctions.h"
#include "Effects/Effectable.h"
#include "Effects/EffectDataAsset.h"
#include "Effects/EffectType.h"
#include "Misc/Guid.h"
#include "Components/ActorComponent.h"
#include "Networking/ZodiaqCharacter.h"
#include "Networking/ZodiaqPlayerState.h"
#include "Spells/SpellManager.h"
#include "Vitals/VitalsContainer.h"

TArray<FDynamicCombatEvent> UDatabaseTimelineInstance::Events;
FString UDatabaseTimelineInstance::MatchId;
FDelegateHandle UDatabaseTimelineInstance::AddedEffectCallback;
FDelegateHandle UDatabaseTimelineInstance::RemovedEffectCallback;
FDelegateHandle UDatabaseTimelineInstance::SpellCallback;
FDelegateHandle UDatabaseTimelineInstance::VitalCallback;

void UDatabaseTimelineInstance::InitTimeline()
{
	Events.Empty();
	MatchId = GenerateRandomMatchId();

	AddedEffectCallback = UEffectable::SrvOnEffectAddedDelegate.AddStatic(OnEffectAdded);
	RemovedEffectCallback = UEffectable::SrvOnEffectRemovedDelegate.AddStatic(OnEffectRemoved);
	SpellCallback = ASpellManager::SrvOnSpellCasted.AddStatic(OnSpellCasted);
	VitalCallback = UVitalsContainer::SrvOnVitalChanged.AddStatic(OnVitalChanged);
}

void UDatabaseTimelineInstance::UnregisterTimeline()
{
	UEffectable::SrvOnEffectAddedDelegate.Remove(AddedEffectCallback);
	UEffectable::SrvOnEffectRemovedDelegate.Remove(RemovedEffectCallback);
	ASpellManager::SrvOnSpellCasted.Remove(SpellCallback);
	UVitalsContainer::SrvOnVitalChanged.Remove(VitalCallback);
}

void UDatabaseTimelineInstance::AddEvent(const FString& Type, const int32 Timestamp, const TSharedPtr<FJsonObject>& Data)
{
	FDynamicCombatEvent NewEvent;
	NewEvent.Timestamp = Timestamp;
	NewEvent.Type = Type;
	NewEvent.Data = Data;

	Events.Add(NewEvent);
}

void UDatabaseTimelineInstance::TimelineEventBossSpawned(const UObject* Sender)
{
	const TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	AddEvent("BossSpawned",GetGameTimeSecondsStatic(Sender) , Json);
}

void UDatabaseTimelineInstance::TimelineEventDefeat(const UObject* Sender)
{
	const TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	AddEvent("Defeat",GetGameTimeSecondsStatic(Sender) , Json);
}

void UDatabaseTimelineInstance::TimelineEventBossKilled(const UObject* Sender)
{
	const TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	AddEvent("BossKilled",GetGameTimeSecondsStatic(Sender) , Json);
}

void UDatabaseTimelineInstance::TimelineEventPlayerJoined(const UObject* Sender, FString PlayerID)
{
	const TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	Json->SetStringField(TEXT("PlayerID"), PlayerID.IsEmpty() ? TEXT("Patric") : PlayerID);
	AddEvent("PlayerSpawned",GetGameTimeSecondsStatic(Sender) , Json);
}

void UDatabaseTimelineInstance::TimelineEventPlayerLeaved(const UObject* Sender, FString PlayerID)
{
	const TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	Json->SetStringField(TEXT("PlayerID"), PlayerID.IsEmpty() ? TEXT("Patric") : PlayerID);
	AddEvent("PlayedLeaved",GetGameTimeSecondsStatic(Sender) , Json);
}

void UDatabaseTimelineInstance::TimelineEventPlayerKilled(const UObject* Sender, const FString& UserName, const FString& IdToken, const FString& SpellId, const int PosX, const int PosY)
{
	TSharedPtr<FJsonObject> CharacterJson = MakeShareable(new FJsonObject);
    
	TSharedPtr<FJsonObject> PositionJson = MakeShareable(new FJsonObject);
    
	CharacterJson->SetStringField("PlayerName", UserName);
	CharacterJson->SetStringField("KillSource", SpellId);
	PositionJson->SetNumberField("PosX", PosX);
	PositionJson->SetNumberField("PosY", PosY);
	CharacterJson->SetObjectField("Position", PositionJson);
	
	AddEvent("Death", GetGameTimeSecondsStatic(Sender), CharacterJson);
}

void UDatabaseTimelineInstance::UploadTimeline(const FString& PlayerIdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
	const FString Path = FString::Printf(TEXT("Analytics/Games/%s"), *MatchId);
	TSharedPtr<FJsonObject> Payload = ConvertToJson();

	UDatabaseFunctions::SetData(Path, Payload, PlayerIdToken, OnSuccess, OnFailure);
}

float UDatabaseTimelineInstance::GetEntityHP(const FString& EntityID, const float& Hp)
{
	static TMap<FString, float> EntitiesHp = {};

	if (EntitiesHp.Contains(EntityID))
	{
		const float OldHp = EntitiesHp[EntityID];
		EntitiesHp[EntityID] = Hp;
		return OldHp;
	}

	EntitiesHp.Add(EntityID, Hp);
	return Hp;
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

void UDatabaseTimelineInstance::OnEffectAdded(UEffectable* Effectable, UEffectDataAsset* Effect, FInstigatorChain& InstigatorChain, FGuid guid) 
{
	//TODO c'est pas encore fini
	const TSharedPtr<FJsonObject> EffectJson = MakeShareable(new FJsonObject);

	EffectJson->SetStringField(TEXT("Guid"), guid.ToString());

	if (const AZodiaqCharacter* CharacterTarget = Cast<AZodiaqCharacter>(Effectable->GetOwner()); CharacterTarget != nullptr) //Player
	{
		const FString Id = CharacterTarget->GetClientData().UUID;
	
		EffectJson->SetStringField(TEXT("Target"), Id.IsEmpty() ? TEXT("Patric") : Id);
	}
	else //BOSS
	{
		EffectJson->SetStringField(TEXT("Target"), "Boss"); //Not beau but osef
	}
	EffectJson->SetStringField(TEXT("Spell name"), Effect->GetName());
	
	const auto OriginActor = InstigatorChain.GetOriginAsActor();

	if (const AZodiaqCharacter* CharacterCaster = Cast<AZodiaqCharacter>(OriginActor); CharacterCaster != nullptr) //Player
	{
		const FString Id = CharacterCaster->GetClientData().UUID;
	
		EffectJson->SetStringField(TEXT("Caster"), Id.IsEmpty() ? TEXT("Patric") : Id);
	}
	else //BOSS
	{
		EffectJson->SetStringField(TEXT("Caster"), "Boss"); //Not beau but osef
	}
	
	switch (Effect->Type)
	{
	case EEffectType::Attack:
		EffectJson->SetStringField(TEXT("Type"), TEXT("Attack"));
		EffectJson->SetNumberField(TEXT("Value"), Effect->GetValue(EEffectValueType::Value));
		break;
	case EEffectType::Defense:
		EffectJson->SetStringField(TEXT("Type"), TEXT("Defense"));
		EffectJson->SetNumberField(TEXT("Value"), Effect->GetValue(EEffectValueType::Value));
		break;
	case EEffectType::MoveSpeed:
		EffectJson->SetStringField(TEXT("Type"), TEXT("MoveSpeed"));
		EffectJson->SetNumberField(TEXT("Value"), Effect->GetValue(EEffectValueType::Value));
		EffectJson->SetNumberField(TEXT("Duration"), Effect->GetValue(EEffectValueType::Duration));
		break;
	case EEffectType::HealBonus:
		EffectJson->SetStringField(TEXT("Type"), TEXT("HealBonus"));
		EffectJson->SetNumberField(TEXT("Value"), Effect->GetValue(EEffectValueType::Value));
		break;
	case EEffectType::Stun:
		EffectJson->SetStringField(TEXT("Type"), TEXT("Stun"));
		EffectJson->SetNumberField(TEXT("Duration"), Effect->GetValue(EEffectValueType::Duration));
		break;
	case EEffectType::Root:
		EffectJson->SetStringField(TEXT("Type"), TEXT("Root"));
		EffectJson->SetNumberField(TEXT("Duration"), Effect->GetValue(EEffectValueType::Duration));
		break;
	case EEffectType::DamageOverTime:
		EffectJson->SetStringField(TEXT("Type"), TEXT("DamageOverTime"));
		EffectJson->SetNumberField(TEXT("Value"), Effect->GetValue(EEffectValueType::Value));
		EffectJson->SetNumberField(TEXT("Duration"), Effect->GetValue(EEffectValueType::Duration));
		EffectJson->SetNumberField(TEXT("Duration"), Effect->GetValue(EEffectValueType::Rate));
		break;
	case EEffectType::HealOverTime:
		EffectJson->SetStringField(TEXT("Type"), TEXT("HealOverTime"));
		EffectJson->SetNumberField(TEXT("Value"), Effect->GetValue(EEffectValueType::Value));
		EffectJson->SetNumberField(TEXT("Rate"), Effect->GetValue(EEffectValueType::Duration));
		EffectJson->SetNumberField(TEXT("Rate"), Effect->GetValue(EEffectValueType::Rate));
		break;
	}
	
	AddEvent("EffectStart",GetGameTimeSecondsStatic(OriginActor) , EffectJson);
}

void UDatabaseTimelineInstance::OnEffectRemoved(UEffectable* Effectable, UEffectDataAsset* Effect, FGuid guid)
{
	const TSharedPtr<FJsonObject> EffectJson = MakeShareable(new FJsonObject);

	EffectJson->SetStringField(TEXT("Guid"), guid.ToString());
	
	AddEvent("EffectEnd",GetGameTimeSecondsStatic(Effectable->GetOwner()) , EffectJson);
}

void UDatabaseTimelineInstance::OnSpellCasted(USpellDataAsset* SpellData, AActor* Sender) 
{
	const TSharedPtr<FJsonObject> EffectJson = MakeShareable(new FJsonObject);

	EffectJson->SetStringField(TEXT("Name"), SpellData->Name.ToString());

	if (const AZodiaqCharacter* CharacterSender = Cast<AZodiaqCharacter>(Sender); CharacterSender != nullptr) //Player
	{
		const FString Id = CharacterSender->GetClientData().UUID;
	
		EffectJson->SetStringField(TEXT("Sender"), Id.IsEmpty() ? TEXT("Patric") : Id);
	}
	else //BOSS
	{
		EffectJson->SetStringField(TEXT("Sender"), "Boss");
	}
	
	AddEvent("Spell",GetGameTimeSecondsStatic(Sender) , EffectJson);
}

void UDatabaseTimelineInstance::OnVitalChanged(UVitalsContainer* Container, EVitalType Type, float Value, float Delta,
	const FInstigatorChain& Chain)
{
	const TSharedPtr<FJsonObject> VitalJson = MakeShareable(new FJsonObject);

	if (const AZodiaqCharacter* CharacterTarget = Cast<AZodiaqCharacter>(Chain.GetOriginAsActor()); CharacterTarget != nullptr) //Player
	{
		const FString Id = CharacterTarget->GetClientData().UUID;
	
		VitalJson->SetStringField(TEXT("Sender"), Id.IsEmpty() ? TEXT("Patric") : Id);
	}
	else //BOSS
	{
		VitalJson->SetStringField(TEXT("Sender"), "Boss");
	}

	if (const AZodiaqCharacter* CharacterTarget = Cast<AZodiaqCharacter>(Container->GetOwner()); CharacterTarget != nullptr) //Player
	{
		const FString Id = CharacterTarget->GetClientData().UUID;
	
		VitalJson->SetStringField(TEXT("Target"), Id.IsEmpty() ? TEXT("Patric") : Id);
	}
	else //BOSS
	{
		VitalJson->SetStringField(TEXT("Target"), "Boss");
	}

	bool IsDead = false;
	
	switch (Type)
	{
	case EVitalType::Shield:
		VitalJson->SetStringField(TEXT("Type"), TEXT("Shield"));
		VitalJson->SetNumberField(TEXT("Type"), Container->GetValue(EVitalType::Shield));
		break;
	case EVitalType::Health:
		VitalJson->SetStringField(TEXT("Type"), TEXT("Health"));
		float NewValue = Container->GetValue(EVitalType::Health);
		VitalJson->SetNumberField(TEXT("Value"), NewValue);
		float OldValue = GetEntityHP(VitalJson->GetStringField("Target"), NewValue);
		if (OldValue == NewValue)
			OldValue = Container->GetMaxValue(EVitalType::Health);
		VitalJson->SetNumberField(TEXT("OldValue"), OldValue);
		VitalJson->SetNumberField(TEXT("Max"), Container->GetMaxValue(EVitalType::Health));
		if (NewValue <= 0)
			IsDead = true;
		break;
	}
	
	// We should use Get identifier and get type everywhere instead of using arbitrary indexes
	const auto ChainSecondElement = Chain.GetElementAt(1);
	if (ChainSecondElement == nullptr)
	{
		AddEvent("Vital",GetGameTimeSecondsStatic(Container->GetOwner()) , VitalJson);
		return;
	}

	const FString SpellId = IInstigatorChainElement::Execute_GetIdentifier(ChainSecondElement);
	
	VitalJson->SetStringField(TEXT("Spell"), SpellId);
	
	AddEvent("Vital",GetGameTimeSecondsStatic(Container->GetOwner()) , VitalJson);

	if (IsDead && VitalJson->GetStringField("Target") != TEXT("Boss"))
	{
		const FVector Position = Chain.GetOriginAsActor()->GetActorLocation();
		TimelineEventPlayerKilled(Container->GetOwner(), VitalJson->GetStringField(TEXT("Target")),
			UDatabaseFunctions::GetIdToken(), SpellId, Position.X, Position.Y);
	}
}

float UDatabaseTimelineInstance::GetGameTimeSecondsStatic(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return 0.0f;

	const UWorld* World = WorldContextObject->GetWorld();
	return World ? World->GetTimeSeconds() : 0.0f;
}
