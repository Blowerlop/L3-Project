// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/BTDecorator_CustomCooldown.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "Engine/World.h"

UBTDecorator_CustomCooldown::UBTDecorator_CustomCooldown(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Cooldown with multiplicator";
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
	CoolDownTime = 5.0f;
	MultiplicatorBlackboardKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CustomCooldown, MultiplicatorBlackboardKey));
	
	// aborting child nodes doesn't makes sense, cooldown starts after leaving this branch
	bAllowAbortChildNodes = false;
}

void UBTDecorator_CustomCooldown::PostLoad()
{
	Super::PostLoad();
	bNotifyTick = (FlowAbortMode != EBTFlowAbortMode::None);
}

bool UBTDecorator_CustomCooldown::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
	const double RecalcTime = (OwnerComp.GetWorld()->GetTimeSeconds() - GetCoolDownTimeWithMultiplicator(OwnerComp));
	return RecalcTime >= DecoratorMemory->LastUseTimestamp;
}

void UBTDecorator_CustomCooldown::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult)
{
	FBTCooldownDecoratorMemory* DecoratorMemory = GetNodeMemory<FBTCooldownDecoratorMemory>(SearchData);
	DecoratorMemory->LastUseTimestamp = SearchData.OwnerComp.GetWorld()->GetTimeSeconds();
	DecoratorMemory->bRequestedRestart = false;
}

void UBTDecorator_CustomCooldown::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
	if (!DecoratorMemory->bRequestedRestart)
	{
		const double RecalcTime = (OwnerComp.GetWorld()->GetTimeSeconds() - GetCoolDownTimeWithMultiplicator(OwnerComp));
		if (RecalcTime >= DecoratorMemory->LastUseTimestamp)
		{
			DecoratorMemory->bRequestedRestart = true;
			OwnerComp.RequestExecution(this);
		}
	}
}

FString UBTDecorator_CustomCooldown::GetStaticDescription() const
{
	// basic info: result after time
	return FString::Printf(TEXT("%s: lock for %.1fs after execution and return %s"), *Super::GetStaticDescription(),
		CoolDownTime, *UBehaviorTreeTypes::DescribeNodeResult(EBTNodeResult::Failed));
}

float UBTDecorator_CustomCooldown::GetCoolDownTimeWithMultiplicator(const UBehaviorTreeComponent& OwnerComp) const
{
	float CooldownTimeWithMultiplicator = CoolDownTime;

	if (MultiplicatorBlackboardKey.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
	{
		const float Multiplicator = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Float>(MultiplicatorBlackboardKey.GetSelectedKeyID());
		CooldownTimeWithMultiplicator *= Multiplicator;
	}

	return CooldownTimeWithMultiplicator;
}

void UBTDecorator_CustomCooldown::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
	const double TimePassed = OwnerComp.GetWorld()->GetTimeSeconds() - DecoratorMemory->LastUseTimestamp;
	
	if (TimePassed < GetCoolDownTimeWithMultiplicator(OwnerComp))
	{
		Values.Add(FString::Printf(TEXT("%s in %ss"),
			(FlowAbortMode == EBTFlowAbortMode::None) ? TEXT("unlock") : TEXT("restart"),
			*FString::SanitizeFloat(GetCoolDownTimeWithMultiplicator(OwnerComp) - TimePassed)));
	}
}

uint16 UBTDecorator_CustomCooldown::GetInstanceMemorySize() const
{
	return sizeof(FBTCooldownDecoratorMemory);
}

void UBTDecorator_CustomCooldown::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	FBTCooldownDecoratorMemory* DecoratorMemory = InitializeNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory, InitType);
	if (InitType == EBTMemoryInit::Initialize)
	{
		DecoratorMemory->LastUseTimestamp = TNumericLimits<double>::Lowest();
	}

	DecoratorMemory->bRequestedRestart = false; ;
}

void UBTDecorator_CustomCooldown::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory, CleanupType);
}

#if WITH_EDITOR

FName UBTDecorator_CustomCooldown::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Decorator.Cooldown.Icon");
}

#endif	// WITH_EDITOR
