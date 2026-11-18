// BTDecorator_ChaseGate.cpp
#include "BTDecorator_ChaseGate.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "StatusEffectComponent.h"
#include "EnemyGameplayTags.h"
#include "AIBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_ChaseGate::UBTDecorator_ChaseGate()
{
	NodeName = TEXT("Chase Gate");
	// Do not observe BB — this is a per-tick query against cached state.
	bNotifyBecomeRelevant  = false;
	bNotifyCeaseRelevant   = false;
	bNotifyTick            = false;
}

bool UBTDecorator_ChaseGate::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AEnemyAIController* AIC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (!AIC) return false;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return false;

	// HasTarget
	if (!BB->GetValueAsObject(BBKeys::TargetActor)) return false;

	// CanSee || HasOverload
	if (AIC->CanSeeTarget()) return true;

	const AEnemyCharacter* Enemy = AIC->GetEnemyCharacter();
	return Enemy && Enemy->GetStatusComponent() &&
		   Enemy->GetStatusComponent()->HasStatus(EnemyTags::Status_SoundOverload);
}

FString UBTDecorator_ChaseGate::GetStaticDescription() const
{
	return TEXT("HasTarget && (CanSee || SoundOverload)");
}