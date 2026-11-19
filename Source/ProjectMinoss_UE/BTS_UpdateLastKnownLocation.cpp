// BTS_UpdateLastKnownLocation.cpp
#include "BTS_UpdateLastKnownLocation.h"
#include "EnemyAIController.h"
#include "AIBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateLastKnownLocation::UBTService_UpdateLastKnownLocation()
{
	NodeName = TEXT("Update Last Known Location");
	bNotifyTick           = true;
	bNotifyBecomeRelevant = false;
	bNotifyCeaseRelevant  = false;
	Interval              = 0.f;    // every frame the branch is active
	RandomDeviation       = 0.f;
}

void UBTService_UpdateLastKnownLocation::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const AEnemyAIController* AIC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (!AIC || !AIC->IsPursuing()) return;

	const AActor* Target = AIC->GetTargetActor();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!Target || !BB) return;

	BB->SetValueAsVector(BBKeys::LastKnownLocation, Target->GetActorLocation());
}

FString UBTService_UpdateLastKnownLocation::GetStaticDescription() const
{
	return TEXT("Write LastKnownLocation while pursuing");
}
