// BTTask_TryKillPlayer.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TryKillPlayer.generated.h"

class UAnimInstance;
class UAnimMontage;

UCLASS(meta=(DisplayName="Try Kill Player"))
class PROJECTMINOSS_UE_API UBTTask_TryKillPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	public:
	UBTTask_TryKillPlayer();

	/** Kill only if the target is within this distance at task start. */
	UPROPERTY(EditAnywhere, Category="Kill", meta=(ClampMin="1.0"))
	float KillRange = 180.f;

	/** Optional kill montage. If null, kill lands immediately. */
	UPROPERTY(EditAnywhere, Category="Kill")
	TObjectPtr<UAnimMontage> KillMontage;

	/** Re-check range again after the montage ends (protects against windup dodges). */
	UPROPERTY(EditAnywhere, Category="Kill")
	bool bRecheckRangeAfterMontage = true;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	private:
	UFUNCTION()
	void OnKillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void ApplyKill();
	void UnbindMontageDelegate();

	UPROPERTY(Transient)
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	UPROPERTY(Transient)
	TObjectPtr<UAnimInstance> CachedAnimInstance;

	TWeakObjectPtr<AActor> CachedTarget;
};