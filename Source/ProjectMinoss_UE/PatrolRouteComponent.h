// PatrolRouteComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PatrolRouteComponent.generated.h"

class APatrolPoint;
class UPuzzleZoneSubsystem;

UCLASS(ClassGroup=(Enemy), meta=(BlueprintSpawnableComponent))
class PROJECTMINOSS_UE_API UPatrolRouteComponent : public UActorComponent
{
	GENERATED_BODY()

	public:
	UPatrolRouteComponent();

	/** Ordered majors. Minors are discovered via each major's MinorPoints array. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Patrol")
	TArray<TObjectPtr<APatrolPoint>> Majors;

	/** Returns the next non-completed patrol point, or nullptr when all are exhausted. */
	APatrolPoint* GetNextPoint(const UPuzzleZoneSubsystem* Zones);

	void ResetIndex();

	private:
	UPROPERTY(Transient)
	TObjectPtr<APatrolPoint> CurrentMajor;

	UPROPERTY(Transient)
	TObjectPtr<APatrolPoint> CurrentPoint;

	int32 CurrentMajorIdx = INDEX_NONE;
	int32 CurrentMinorIdx = INDEX_NONE;

	APatrolPoint* AdvanceMinor(const UPuzzleZoneSubsystem* Zones);
	APatrolPoint* AdvanceMajor(const UPuzzleZoneSubsystem* Zones);
};