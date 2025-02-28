#pragma once

UENUM(Blueprintable)
enum class EEffectType : uint8
{
	DamageResistance,
	DamageWeakness,
	HealBonus,
	Stun,
	Slow,
	Root,
	DamageOverTime,
	HealOverTime
};

UENUM(Blueprintable)
enum class EEffectValueType : uint8
{
	Value,
	Rate,
	Duration
};



