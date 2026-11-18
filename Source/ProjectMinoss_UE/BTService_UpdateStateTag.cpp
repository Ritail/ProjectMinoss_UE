// BTService_UpdateStateTag.cpp
#include "BTService_UpdateStateTag.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "AIBlackboardKeys.h"
#include "BlackboardKeyType_GameplayTag.h"
#include "BehaviorTree/BlackboardComponent.h"

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

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	BB->SetValue<UBlackboardKeyType_GameplayTag>(BBKeys::StateTag, StateTag);

	if (const AEnemyAIController* AIC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner()))
	{
		if (AEnemyCharacter* Enemy = AIC->GetEnemyCharacter())
		{
			Enemy->RecalculateMovementStats();
		}
	}
}

FString UBTService_UpdateStateTag::GetStaticDescription() const
{
	return StateTag.IsValid()
		? FString::Printf(TEXT("State = %s"), *StateTag.ToString())
		: TEXT("State = (unset)");
}