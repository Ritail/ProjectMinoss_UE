// BTService_UpdateLastKnownLocation.cpp
#include "BTS_UpdateLastKnownLocation.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "StatusEffectComponent.h"
#include "EnemyGameplayTags.h"
#include "AIBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateLastKnownLocation::UBTService_UpdateLastKnownLocation()
{
	NodeName = TEXT("Update Last Known Location");
	bNotifyTick       = true;
	bNotifyBecomeRelevant = false;
	bNotifyCeaseRelevant  = false;
	Interval          = 0.f;    // every frame the branch is active
	RandomDeviation   = 0.f;
}

void UBTService_UpdateLastKnownLocation::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const AEnemyAIController* AIC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (!AIC) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(BBKeys::TargetActor));
	if (!Target) return;

	const AEnemyCharacter* Enemy = AIC->GetEnemyCharacter();
	const bool bOverload = Enemy && Enemy->GetStatusComponent() &&
						   Enemy->GetStatusComponent()->HasStatus(EnemyTags::Status_SoundOverload);

	// Write LKP when we can see (fresh sight data) OR when overload keeps us tracking.
	if (AIC->CanSeeTarget() || bOverload)
	{
		BB->SetValueAsVector(BBKeys::LastKnownLocation, Target->GetActorLocation());
	}
}

FString UBTService_UpdateLastKnownLocation::GetStaticDescription() const
{
	return TEXT("Write LastKnownLocation when (CanSee || SoundOverload)");
}