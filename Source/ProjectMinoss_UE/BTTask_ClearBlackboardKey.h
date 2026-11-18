// BTTask_ClearBlackboardKey.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ClearBlackboardKey.generated.h"

UCLASS(meta=(DisplayName="Clear Blackboard Key"))
class PROJECTMINOSS_UE_API UBTTask_ClearBlackboardKey : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	public:
	UBTTask_ClearBlackboardKey();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};