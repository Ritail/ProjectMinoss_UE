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
class UNoiseQueueComponent;

UCLASS()
class PROJECTMINOSS_UE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	public:
	AEnemyAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	/** Cached flag, updated by sight stimuli. */
	bool CanSeeTarget() const { return bCanSeeTarget; }

	/** True while the SoundOverload status is active. */
	bool HasSoundOverload() const;

	/** The actor the enemy is currently tracking, or null. Reads the TargetActor key. */
	AActor* GetTargetActor() const;

	/**
	 * True while the enemy is in pursuit: it has a target and either sees it
	 * or is overloaded by noise. Used by the Chase gate and the LKL service.
	 */
	bool IsPursuing() const;

	/** Writes the StateTag key and recalculates movement speed. */
	void SetAIState(FGameplayTag NewState);

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
	float BaseSightRadius = 0.f;

	UPROPERTY()
	TObjectPtr<UNoiseQueueComponent> NoiseQueueComp;

	void ConfigurePerception();
	void SetSightEnabled(bool bEnabled);
	void RecalculateSightRange();
	void ApplyRageIfThresholdMet();

	/** Single write path for TargetActor. Passing null clears it. */
	void SetTargetActor(AActor* Target);

	void BindPawnDelegates();
	void UnbindPawnDelegates();

	UBehaviorTreeComponent* GetBTComponent() const;

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleZoneCompleted(FGameplayTag ZoneId);

	void HandleStatusAdded(FGameplayTag Tag, UObject* MyInstigator);
	void HandleStatusRemoved(FGameplayTag Tag, EStatusRemovalReason Reason);
	void HandleNoiseOverload(AActor* MyInstigator);
	void HandleInvestigateLocationChanged(FVector Location, bool bValid);
};
