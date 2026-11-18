// PatrolPoint.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "PatrolPoint.generated.h"

UENUM(BlueprintType)
enum class EPatrolPointType : uint8
{
	Major,
	Minor
};

UCLASS(Blueprintable)
class PROJECTMINOSS_UE_API APatrolPoint : public AActor
{
	GENERATED_BODY()

	public:
	APatrolPoint();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Patrol")
	FGameplayTag ZoneId;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Patrol")
	EPatrolPointType PointType = EPatrolPointType::Minor;

	/** Only populated when PointType == Major. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Patrol",
			  meta=(EditCondition="PointType==EPatrolPointType::Major", EditConditionHides))
	TArray<TObjectPtr<APatrolPoint>> MinorPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Patrol")
	TObjectPtr<USceneComponent> SceneRoot;
};