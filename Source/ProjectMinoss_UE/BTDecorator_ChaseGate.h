// BTDecorator_ChaseGate.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ChaseGate.generated.h"

UCLASS(meta=(DisplayName="Chase Gate"))
class PROJECTMINOSS_UE_API UBTDecorator_ChaseGate : public UBTDecorator
{
	GENERATED_BODY()

	public:
	UBTDecorator_ChaseGate();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};