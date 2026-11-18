// BTTask_GetNextPatrolPoint.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetNextPatrolPoint.generated.h"

UCLASS(meta=(DisplayName="Get Next Patrol Point"))
class PROJECTMINOSS_UE_API UBTTask_GetNextPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

	public:
	UBTTask_GetNextPatrolPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};