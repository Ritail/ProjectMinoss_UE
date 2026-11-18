// BTTask_GetNextPatrolPoint.cpp
#include "BTTask_GetNextPatrolPoint.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "PatrolRouteComponent.h"
#include "PatrolPoint.h"
#include "PuzzleZoneSubsystem.h"
#include "AIBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"

UBTTask_GetNextPatrolPoint::UBTTask_GetNextPatrolPoint()
{
	NodeName = TEXT("Get Next Patrol Point");
}

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AEnemyAIController* AIC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (!AIC) return EBTNodeResult::Failed;

	AEnemyCharacter* Enemy = AIC->GetEnemyCharacter();
	if (!Enemy || !Enemy->GetPatrolComponent()) return EBTNodeResult::Failed;

	UWorld* World = OwnerComp.GetWorld();
	UPuzzleZoneSubsystem* Zones = World ? World->GetSubsystem<UPuzzleZoneSubsystem>() : nullptr;
	if (!Zones) return EBTNodeResult::Failed;

	APatrolPoint* Next = Enemy->GetPatrolComponent()->GetNextPoint(Zones);
	if (!Next)
	{
		// No valid points remain — let the Patrol branch's inner selector
		// fall through to the Home-idle sequence (§8.2).
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	BB->SetValueAsObject(BBKeys::CurrentPatrolPoint, Next);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_GetNextPatrolPoint::GetStaticDescription() const
{
	return TEXT("Query UPatrolRouteComponent → write CurrentPatrolPoint");
}