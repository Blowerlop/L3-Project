#include "EffectSystemConfiguration.h"

#include "EffectResolverParams.h"

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
