#include "Utilities/Blackboard/UBlackboard.h"
#include "BehaviorTree/BlackboardComponent.h"


TArray<FName> UBlackboard::GetAllKeys(const UBlackboardComponent* blackboard)
{
	TArray<FName> keys;

	if (!blackboard)
	{
		return keys;
	}

	const UBlackboardData* blackboardData = blackboard->GetBlackboardAsset();
	if (!blackboardData)
	{
		return keys;
	}

	for (const FBlackboardEntry& entry : blackboardData->Keys)
	{
		keys.Add(entry.EntryName);
	}

	return keys;
}
