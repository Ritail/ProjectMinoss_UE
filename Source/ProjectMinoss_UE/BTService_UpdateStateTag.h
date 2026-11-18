// BTService_UpdateStateTag.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GameplayTagContainer.h"
#include "BTService_UpdateStateTag.generated.h"

UCLASS(meta=(DisplayName="Update State Tag"))
class PROJECTMINOSS_UE_API UBTService_UpdateStateTag : public UBTService
{
	GENERATED_BODY()

	public:
	UBTService_UpdateStateTag();

	/** Tag to write into the StateTag key when this branch becomes active. */
	UPROPERTY(EditAnywhere, Category="State")
	FGameplayTag StateTag;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};