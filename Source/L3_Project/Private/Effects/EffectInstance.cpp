#include "Effects/EffectInstance.h"

#include "Effects/EffectDataAsset.h"

#include "TimerManager.h"
#include "Effects/Effectable.h"
#include "Effects/EffectType.h"

void UEffectInstance::Init(UEffectDataAsset* EffectAsset, AActor* EffectApplier, UEffectable* EffectParent)
{
	Data = EffectAsset;
	Applier = EffectApplier;
	Parent = EffectParent;
	
	if (Data->bUseDuration)
	{
		GetWorld()->GetTimerManager().SetTimer(LifetimeTimerHandle, this, &UEffectInstance::OnTimerEnded,
			Data->GetValue(EEffectValueType::Duration));
	}
}

void UEffectInstance::BeginDestroy()
{
	UObject::BeginDestroy();

	// todo: Check thisp ls
	if (LifetimeTimerHandle.IsValid())
	{
		LifetimeTimerHandle.Invalidate();
	}
}

void UEffectInstance::OnTimerEnded()
{
	Parent->RemoveEffect(this);
}
