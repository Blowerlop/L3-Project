#pragma once

#include "CoreMinimal.h"
#include "Database/DatabaseFunctions.h"
#include "Dom/JsonObject.h"
#include "Effects/Effectable.h"
#include "DatabaseTimelineInstance.generated.h"

enum class EVitalType : uint8;
class USpellController;
class USpellDataAsset;
class UVitalsContainer;

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
class L3_PROJECT_API UDatabaseTimelineInstance : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Timeline")
	static void InitTimeline();
	UFUNCTION(BlueprintCallable, Category = "Timeline")
	static void UnregisterTimeline();

	/**
	 * Add event to timeline
	 * @param Type - Event Type
	 * @param Timestamp - Event time                                                                                                                                                                       What a useless comment
	 * @param Data - Event JSon Data
	 */
	static void AddEvent(const FString& Type, int32 Timestamp, const TSharedPtr<FJsonObject>& Data);

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Sender"), Category = "Timeline")
	static void TimelineEventBossSpawned(const UObject* Sender);
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Sender"), Category = "Timeline")
	static void TimelineEventDefeat(const UObject* Sender);
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Sender"), Category = "Timeline")
	static void TimelineEventBossKilled(const UObject* Sender);
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Sender"), Category = "Timeline")
	static void TimelineEventPlayerJoined(const UObject* Sender, FString PlayerID);
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Sender"), Category = "Timeline")
	static void TimelineEventPlayerLeaved(const UObject* Sender, FString PlayerID);
	//Add new Death
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	static void TimelineEventPlayerKilled(const UObject* Sender, const FString& UserName, const FString& IdToken, const FString& SpellId, int PosX, int PosY);

	/**
	 * Send data to DB (HOST ONLY) And should be done at the end of the game for pity
	 */
	UFUNCTION(BlueprintCallable, Category = "Timeline")
	static void UploadTimeline(const FString& PlayerIdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

private:
	static float GetEntityHP(const FString& EntityID, const float& Hp);
	
	static TArray<FDynamicCombatEvent> Events;
	static FString MatchId;
	static FDelegateHandle AddedEffectCallback;
	static FDelegateHandle RemovedEffectCallback;
	static FDelegateHandle SpellCallback;
	static FDelegateHandle VitalCallback;

	static TSharedPtr<FJsonObject> ConvertToJson();

	static FString GenerateRandomMatchId();
	static void OnEffectAdded(UEffectable* Effectable, UEffectDataAsset* Effect, FInstigatorChain& InstigatorChain, FGuid guid);
	static void OnEffectRemoved(UEffectable* Effectable, UEffectDataAsset* Effect, FGuid guid);
	static void OnSpellCasted(USpellDataAsset* SpellData, AActor* Sender);
	static void OnVitalChanged(UVitalsContainer* Container, EVitalType Type, float Value, float Delta, const FInstigatorChain& Chain);
	static float GetGameTimeSecondsStatic(const UObject* WorldContextObject);
};

/*Data type (Blib change/update si tu as besoin)
 *
 *All events :
 *
 *TYPE => Le Type
 *TIME => Le moment de l'invocation
 *DATA => Les datas
 *
 *	////////////////////////////////////////////////////////////////////////////////////////////////////
 *
 *Player Damage:  *dégats qu'un joueur inflige*
 *
 *TYPE => "PlayerDamage"
 *TIME => (?)
 *DATA =>
 *	"PlayerID" => id du lanceur
 *	"SpellID"  => id du sort lancé
 *	"Damage"   => dégats infligés
 *	"Type"     => type de spell (Skillshot, Zone, DOT...)
 *
 *	////////////////////////////////////////////////////////////////////////////////////////////////////
 *
 *Player Buff:  *effet qu'un joueur propage (Positif)*
 *
 *TYPE => "PlayerBuff"
 *TIME => (?)
 *DATA =>
 *	"PlayerID" => id du lanceur
 *	"TargetID" => id du joueur qui reçoit le buff
 *	"SpellID"  => id du sort lancé
 *	"BuffType" => type de buff ("health", "damage", "speed", "shield"...)
 *	"Amount"   => valeur du buff ^
 *	"Type"     => type de spell (Skillshot, Zone, DOT...)
 *
 *	////////////////////////////////////////////////////////////////////////////////////////////////////
 *
 *Boss Damage:  *dégat du boss*
 *
 *TYPE => "BossDamage"
 *TIME => (?)
 *DATA =>
 *	"TargetID" => id du joueur qui reçoit les dmg
 *	"SpellID"  => id du sort lancé
 *	"Damage"   => dégats infligés
 *	"Type"     => type de spell (Skillshot, Zone, DOT...)
 */