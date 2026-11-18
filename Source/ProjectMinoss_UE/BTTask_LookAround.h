// BTTask_LookAround.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LookAround.generated.h"

UCLASS(meta=(DisplayName="Look Around"))
class PROJECTMINOSS_UE_API UBTTask_LookAround : public UBTTaskNode
{
	GENERATED_BODY()

	public:
	UBTTask_LookAround();

	/** Duration of the sweep. */
	UPROPERTY(EditAnywhere, Category="Look", meta=(ClampMin="0.1"))
	float Duration = 2.0f;

	/** Peak yaw offset from starting rotation (degrees). Sin-wave scan. */
	UPROPERTY(EditAnywhere, Category="Look", meta=(ClampMin="1.0"))
	float SweepAngleDegrees = 60.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
};

struct FBTLookAroundMemory
{
	FRotator StartRotation = FRotator::ZeroRotator;
	float    ElapsedTime = 0.f;
	float    Duration = 0.f;
};