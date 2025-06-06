#include "Effects/EffectSystemConfiguration.h"

#include "Effects/EffectResolverParams.h"
#include "Effects/EffectType.h"

bool UEffectSystemConfiguration::NeedDuration(const EEffectType Type)
{
	switch (Type)
	{
	case EEffectType::Attack:
	case EEffectType::Defense:
	case EEffectType::MoveSpeed:
	case EEffectType::HealBonus:
	case EEffectType::Stun:
	case EEffectType::Root:
	case EEffectType::DamageOverTime:
	case EEffectType::HealOverTime:
		return true;
	}

	return false;
}

bool UEffectSystemConfiguration::NeedValue(const EEffectType Type)
{
	switch (Type)
	{
	case EEffectType::Attack:
	case EEffectType::Defense:
	case EEffectType::MoveSpeed:
	case EEffectType::HealBonus:
	case EEffectType::DamageOverTime:
	case EEffectType::HealOverTime:
		return true;
	case EEffectType::Stun:
	case EEffectType::Root:
		return false;
	}

	return false;
}

bool UEffectSystemConfiguration::NeedRate(const EEffectType Type)
{
	switch (Type)
	{
	case EEffectType::Attack:
	case EEffectType::Defense:
	case EEffectType::MoveSpeed:
	case EEffectType::HealBonus:
	case EEffectType::Stun:
	case EEffectType::Root:
		return false;
	case EEffectType::DamageOverTime:
	case EEffectType::HealOverTime:
		return true;
	}

	return false;
}

#if WITH_EDITOR
void UEffectSystemConfiguration::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for(auto& Tuple : ResolversByType)
	{
		if (!IsValid(Tuple.Value.ResolverClass)) continue;
		
		const auto ParamsType = ParamsByResolver.FindRef(Tuple.Value.ResolverClass);

		if (!IsValid(ParamsType)) continue;

		if (Tuple.Value.Params == nullptr || Tuple.Value.Params->GetClass() != *ParamsType)
			Tuple.Value.Params = NewObject<UEffectResolverParams>(this, *ParamsType);
	}
}
#endif
