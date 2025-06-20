#pragma once

UENUM(Blueprintable)
enum class EEffectType : uint8
{
	Attack,
	Defense,
	MoveSpeed,
	HealBonus,
	Stun,
	Root,
	DamageOverTime,
	HealOverTime,
	EasyMode
};

UENUM(Blueprintable)
enum class EEffectValueType : uint8
{
	Value,
	Rate,
	Duration
};

UENUM(Blueprintable)
enum class EEffectImpact : uint8
{
	Neutral,
	Good,
	Bad
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ESpellAction : uint8
{
	NONE	   = 0 UMETA(Hidden),
	Debuff	   = 1 << 0,
	Cleanse	   = 1 << 1,
};
ENUM_CLASS_FLAGS(ESpellAction);



