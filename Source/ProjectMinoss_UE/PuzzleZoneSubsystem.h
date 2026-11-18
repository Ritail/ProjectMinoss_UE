// PuzzleZoneSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "PuzzleZoneSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoneCompleted, FGameplayTag, ZoneId);
DECLARE_MULTICAST_DELEGATE(FOnZonesReset);

UCLASS()
class PROJECTMINOSS_UE_API UPuzzleZoneSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	public:
	// UWorldSubsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// World-state API
	UFUNCTION(BlueprintCallable, Category="Puzzle")
	void MarkZoneCompleted(FGameplayTag ZoneId);

	UFUNCTION(BlueprintPure, Category="Puzzle")
	bool IsZoneCompleted(FGameplayTag ZoneId) const;

	UFUNCTION(BlueprintPure, Category="Puzzle")
	int32 GetCompletedCount() const;

	UFUNCTION(BlueprintPure, Category="Puzzle")
	int32 GetTotalZoneCount() const { return TotalZoneCount; }

	/** Full reset — used by AEnemyGameMode only. */
	UFUNCTION(BlueprintCallable, Category="Puzzle")
	void Reset();

	/** Fires when a new zone is completed (BP-friendly: UI, audio). */
	UPROPERTY(BlueprintAssignable, Category="Puzzle")
	FOnZoneCompleted OnZoneCompleted;

	/** Internal reset notification for C++ listeners. NOT the game-reset event. */
	FOnZonesReset OnZonesReset;

	private:
	UPROPERTY()
	TSet<FGameplayTag> CompletedZones;

	int32 TotalZoneCount = 0;
};