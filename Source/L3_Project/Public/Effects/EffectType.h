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
	HealOverTime
};

UENUM(Blueprintable)
enum class EEffectValueType : uint8
{
	Value,
	Rate,
	Duration
};



