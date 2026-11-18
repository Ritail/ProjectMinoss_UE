// BTDecorator_HasStatusTag.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayTagContainer.h"
#include "BTDecorator_HasStatusTag.generated.h"

UCLASS(meta=(DisplayName="Has Status Tag"))
class PROJECTMINOSS_UE_API UBTDecorator_HasStatusTag : public UBTDecorator
{
	GENERATED_BODY()

	public:
	UBTDecorator_HasStatusTag();

	UPROPERTY(EditAnywhere, Category="Status")
	FGameplayTag StatusTag;

	UPROPERTY(EditAnywhere, Category="Status")
	bool bInvert = false;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};