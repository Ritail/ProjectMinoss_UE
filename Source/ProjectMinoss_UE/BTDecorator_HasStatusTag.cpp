// BTDecorator_HasStatusTag.cpp
#include "BTDecorator_HasStatusTag.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "StatusEffectComponent.h"

UBTDecorator_HasStatusTag::UBTDecorator_HasStatusTag()
{
	NodeName = TEXT("Has Status Tag");
}

bool UBTDecorator_HasStatusTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AEnemyAIController* AIC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (!AIC) return false;

	const AEnemyCharacter* Enemy = AIC->GetEnemyCharacter();
	if (!Enemy || !Enemy->GetStatusComponent()) return false;

	const bool bHas = Enemy->GetStatusComponent()->HasStatus(StatusTag);
	return bInvert ? !bHas : bHas;
}

FString UBTDecorator_HasStatusTag::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s%s"),
		*Super::GetStaticDescription(),
		bInvert ? TEXT("NOT ") : TEXT(""),
		*StatusTag.ToString());
}