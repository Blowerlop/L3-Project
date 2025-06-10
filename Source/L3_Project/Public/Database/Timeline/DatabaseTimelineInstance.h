#pragma once

#include "CoreMinimal.h"
#include "Database/DatabaseFunctions.h"
#include "Dom/JsonObject.h"
#include "DatabaseTimelineInstance.generated.h"

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
	static void Init();

	/**
	 * Add event to timeline
	 * @param Type - Event Type
	 * @param Timestamp - Event time                                                                                                                                                                       What a useless comment
	 * @param Data - Event JSon Data
	 */
	static void AddEvent(const FString& Type, int32 Timestamp, const TSharedPtr<FJsonObject>& Data);

	/**
	 * Send data to DB (HOST ONLY) And should be done at the end of the game for pity
	 */
	static void UploadTimeline(const FString& PlayerIdToken, const FSuccess& OnSuccess, const FFailed& OnFailure);

private:
	static TArray<FDynamicCombatEvent> Events;
	static FString MatchId;

	static TSharedPtr<FJsonObject> ConvertToJson() const;

	static FString GenerateRandomMatchId();
	static void OnEffectAdded(UEffectable* effectable, UEffectDataAsset* effect, AActor* actor, FGuid guid);
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