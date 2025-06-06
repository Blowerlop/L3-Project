#include "Effects/EffectDataAsset.h"

#include "Effects/EffectStackingBehaviour.h"
#include "Effects/EffectSystemConfiguration.h"
#include "Effects/EffectType.h"

TArray<EEffectValueType> GetValueTypes(const EEffectType EffectType, const bool bUseDuration)
{
	TArray<EEffectValueType> Array = {};

	if (UEffectSystemConfiguration::NeedValue(EffectType)) Array.Add(EEffectValueType::Value);
	if (UEffectSystemConfiguration::NeedRate(EffectType)) Array.Add(EEffectValueType::Rate);
	if (UEffectSystemConfiguration::NeedDuration(EffectType) && bUseDuration) Array.Add(EEffectValueType::Duration);

	return Array;
}

float UEffectDataAsset::GetValue(const EEffectValueType ValueType) const
{
	return Values.FindRef(ValueType);
}

#if WITH_EDITOR
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

	bNeedStacking = UEffectSystemConfiguration::NeedValue(Type);

	if (!bNeedStacking && StackingBehaviour) 
	{
		StackingBehaviour = nullptr;
	}
}
#endif