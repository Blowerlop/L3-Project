#include "Effects/EffectStackingBehaviour.h"

#include "Stats/StatsContainer.h"

float UEffectStackingBehaviour::GetModifiedValue(float BaseValue, AActor* Applier) const
{
	if (IsModifierEnabled(EStackingBehaviourModifiers::Attack))
	{
		if (const auto StatsContainer = Applier->GetComponentByClass<UStatsContainer>(); IsValid(StatsContainer))
		{
			BaseValue *= StatsContainer->GetValue(EGameStatType::Attack);
		}
	}

	return BaseValue;
}
