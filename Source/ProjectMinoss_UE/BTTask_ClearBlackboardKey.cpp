// BTTask_ClearBlackboardKey.cpp
#include "BTTask_ClearBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBlackboardKey::UBTTask_ClearBlackboardKey()
{
	NodeName = TEXT("Clear Blackboard Key");
}

EBTNodeResult::Type UBTTask_ClearBlackboardKey::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB || !BlackboardKey.IsSet()) return EBTNodeResult::Failed;

	BB->ClearValue(BlackboardKey.GetSelectedKeyID());
	return EBTNodeResult::Succeeded;
}

FString UBTTask_ClearBlackboardKey::GetStaticDescription() const
{
	return BlackboardKey.IsSet()
		? FString::Printf(TEXT("Clear %s"), *BlackboardKey.SelectedKeyName.ToString())
		: TEXT("Clear (no key selected)");
}