#include "Spells/SpellDataAsset.h"

#include "Spells/SpellAimer.h"
#include "Spells/SpellAimerParams.h"

FName USpellDataAsset::GetMontageSectionName() const
{
	return FName(*FString::Printf(TEXT("Combo_%d"), ComboIndex));
}

void USpellDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!Aimer) return;
	
	if (const auto DefaultObject = Aimer.GetDefaultObject())
	{
		if (!DefaultObject->AimerParamsClass)
		{
			AimerParams = nullptr;
			return;
		}
		
		if (AimerParams && AimerParams->GetClass() == DefaultObject->AimerParamsClass.Get())
			return;
			
		AimerParams = NewObject<USpellAimerParams>(this, DefaultObject->AimerParamsClass);
	}
}
