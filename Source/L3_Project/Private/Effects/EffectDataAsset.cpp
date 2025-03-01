#include "Effects/EffectDataAsset.h"

#include "Effects/EffectType.h"

bool NeedDuration(const EEffectType EffectType)
{
	switch (EffectType)
	{
	case EEffectType::DamageResistance:
	case EEffectType::DamageWeakness:
	case EEffectType::HealBonus:
	case EEffectType::Stun:
	case EEffectType::Slow:
	case EEffectType::Root:
	case EEffectType::DamageOverTime:
	case EEffectType::HealOverTime:
		return true;
	}

	return false;
}

bool NeedValue(const EEffectType EffectType)
{
	switch (EffectType)
	{
	case EEffectType::DamageResistance:
	case EEffectType::DamageWeakness:
	case EEffectType::HealBonus:
	case EEffectType::Slow:
	case EEffectType::DamageOverTime:
	case EEffectType::HealOverTime:
		return true;
	case EEffectType::Stun:
	case EEffectType::Root:
		return false;
	}

	return false;
}

bool NeedRate(const EEffectType EffectType)
{
	switch (EffectType)
	{
	case EEffectType::DamageResistance:
	case EEffectType::DamageWeakness:
	case EEffectType::HealBonus:
	case EEffectType::Stun:
	case EEffectType::Slow:
	case EEffectType::Root:
		return false;
	case EEffectType::DamageOverTime:
	case EEffectType::HealOverTime:
		return true;
	}

	return false;
}

TArray<EEffectValueType> GetValueTypes(const EEffectType EffectType, const bool bUseDuration)
{
	TArray<EEffectValueType> Array = {};

	if (NeedValue(EffectType)) Array.Add(EEffectValueType::Value);
	if (NeedRate(EffectType)) Array.Add(EEffectValueType::Rate);
	if (NeedDuration(EffectType) && bUseDuration) Array.Add(EEffectValueType::Duration);

	return Array;
}

float UEffectDataAsset::GetValue(const EEffectValueType ValueType) const
{
	return Values.FindRef(ValueType);
}

void UEffectDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Types = GetValueTypes(Type, bUseDuration);

	TArray<EEffectValueType> Keys{};
	Values.GetKeys(Keys);

	for (auto& ValueType : Keys)
	{
		if (!Types.Contains(ValueType))
		{
			Values.Remove(ValueType);
			continue;
		}

		Types.Remove(ValueType);
	}

	for (auto& ValueType : Types)
	{
		Values.Add(ValueType, 0.f);
	}
}
