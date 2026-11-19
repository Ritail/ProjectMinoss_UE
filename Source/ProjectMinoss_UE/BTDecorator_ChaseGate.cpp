// BTDecorator_ChaseGate.cpp
#include "BTDecorator_ChaseGate.h"
#include "EnemyAIController.h"
#include "AIBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"

UBTDecorator_ChaseGate::UBTDecorator_ChaseGate()
{
	NodeName = TEXT("Chase Gate");

	// Observe TargetActor. The engine only re-evaluates a higher-priority
	// branch when something it observes changes. The old tick-based gate only
	// requested re-evaluation when it turned false, so a Chase branch that
	// became valid while Patrol was running never interrupted Patrol.
	// Abort on the node must be Both or Lower Priority.
	BlackboardKey.SelectedKeyName = BBKeys::TargetActor;
	BlackboardKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(UBTDecorator_ChaseGate, BlackboardKey),
		AActor::StaticClass());
}

bool UBTDecorator_ChaseGate::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AEnemyAIController* AIC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	return AIC && AIC->IsPursuing();
}

FString UBTDecorator_ChaseGate::GetStaticDescription() const
{
	return TEXT("HasTarget && (CanSee || SoundOverload)");
}
