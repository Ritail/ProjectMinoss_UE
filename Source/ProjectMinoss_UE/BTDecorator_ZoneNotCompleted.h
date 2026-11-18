// BTDecorator_ZoneNotCompleted.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayTagContainer.h"
#include "BTDecorator_ZoneNotCompleted.generated.h"

UCLASS(meta=(DisplayName="Zone Not Completed"))
class PROJECTMINOSS_UE_API UBTDecorator_ZoneNotCompleted : public UBTDecorator
{
	GENERATED_BODY()

	public:
	UBTDecorator_ZoneNotCompleted();

	/** If invalid, decorator reads ZoneId from the CurrentPatrolPoint actor. */
	UPROPERTY(EditAnywhere, Category="Zone")
	FGameplayTag ZoneIdOverride;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};