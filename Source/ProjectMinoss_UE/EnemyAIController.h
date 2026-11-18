// EnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Perception/AIPerceptionTypes.h"
#include "StatusEffectComponent.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UBehaviorTreeComponent;
class AEnemyCharacter;

UCLASS()
class PROJECTMINOSS_UE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	public:
	AEnemyAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	/** Cached flag — updated on sight stimuli. Used by BTDecorator_ChaseGate. */
	bool CanSeeTarget() const { return bCanSeeTarget; }

	AEnemyCharacter* GetEnemyCharacter() const;

	static constexpr uint8 PlayerTeamId = 1;
	static constexpr uint8 EnemyTeamId  = 2;
	
	/** GameMode calls this first. */
	void PrepareForReset();

	/** GameMode calls this after player + zones are reset. */
	void ResumeAfterReset();

	protected:
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	FGenericTeamId TeamId = FGenericTeamId(EnemyTeamId);

	private:
	bool bCanSeeTarget = false;

	void ConfigurePerception();
	void SetSightEnabled(bool bEnabled);
	void RecalculateSightRange();
	void ApplyRageIfThresholdMet();
	
	UBehaviorTreeComponent* GetBTComponent() const;

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleZoneCompleted(FGameplayTag ZoneId);

	void HandleStatusAdded(FGameplayTag Tag, UObject* MyInstigator);
	void HandleStatusRemoved(FGameplayTag Tag, EStatusRemovalReason Reason);
	void HandleNoiseOverload(AActor* MyInstigator);
	void HandleInvestigateLocationChanged(FVector Location, bool bValid);

	float BaseSightRadius = 0.f;
};