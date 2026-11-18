// EnemyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "StatusEffectComponent.h"
#include "EnemyCharacter.generated.h"

class UStatusEffectComponent;
class UPatrolRouteComponent;
struct FGameplayTag;

UCLASS()
class PROJECTMINOSS_UE_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

	public:
	AEnemyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Components")
	TObjectPtr<UStatusEffectComponent> StatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Components")
	TObjectPtr<UPatrolRouteComponent> PatrolComponent;

	/** Cached at BeginPlay. Fallback destination when no patrol points remain. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Enemy")
	FVector HomeLocation = FVector::ZeroVector;

	UFUNCTION(BlueprintPure, Category="Enemy|Components")
	UStatusEffectComponent* GetStatusComponent() const { return StatusComponent; }

	UFUNCTION(BlueprintPure, Category="Enemy|Components")
	UPatrolRouteComponent* GetPatrolComponent() const { return PatrolComponent; }

	UFUNCTION(BlueprintPure, Category="Enemy")
	FVector GetHomeLocation() const { return HomeLocation; }

	/** Called by the controller after RunBehaviorTree to bootstrap the character side. */
	void OnAIPossessed(class AAIController* AIC);

	/** Composes MaxWalkSpeed from current StateTag + status multipliers. Called by BT service and status listeners. */
	void RecalculateMovementStats();

	protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

	private:
	void HandleStatusAdded(FGameplayTag Tag, UObject* Instigator);
	void HandleStatusRemoved(FGameplayTag Tag, EStatusRemovalReason Reason);
};