#include "Effects/EffectResolver.h"

#include "Effects/Effectable.h"

float UEffectResolver::GetTotalValueForContainer(const FEffectValueContainer& Container)
{
	float TotalValue = 0.0f;
	for (const auto& Pair : Container.Values)
	{
		TotalValue += Pair.Value;
	}
		
	return TotalValue;
}
