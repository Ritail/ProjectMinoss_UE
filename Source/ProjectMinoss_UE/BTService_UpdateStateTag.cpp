// BTService_UpdateStateTag.cpp
#include "BTService_UpdateStateTag.h"
#include "EnemyAIController.h"

UBTService_UpdateStateTag::UBTService_UpdateStateTag()
{
	NodeName = TEXT("Update State Tag");
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant  = false;
	bNotifyTick           = false;   // no tick
	Interval              = 0.f;
	RandomDeviation       = 0.f;
}

void UBTService_UpdateStateTag::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (!StateTag.IsValid()) return;

	// The controller owns state changes: it writes the key and recalculates speed.
	if (AEnemyAIController* AIC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner()))
	{
		AIC->SetAIState(StateTag);
	}
}

FString UBTService_UpdateStateTag::GetStaticDescription() const
{
	return StateTag.IsValid()
		? FString::Printf(TEXT("State = %s"), *StateTag.ToString())
		: TEXT("State = (unset)");
}
