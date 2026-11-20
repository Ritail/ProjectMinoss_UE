// EnemyGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnemyGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameReset);

UCLASS()
class PROJECTMINOSS_UE_API AEnemyGameMode : public AGameModeBase
{
	GENERATED_BODY()

	public:
	AEnemyGameMode();

	/** Full reset — ordered per doc §10.1. Safe to call repeatedly. */
	UFUNCTION(BlueprintCallable, Category="Game")
	void ResetGame();

	/** Fires exactly once at the end of ResetGame. UI/audio/VFX bind here. */
	UPROPERTY(BlueprintAssignable, Category="Game")
	FOnGameReset OnGameReset;
	
	UFUNCTION(Exec)
	void Debug_CompleteZone(FName ZoneTagName);

	protected:
	virtual void StartPlay() override;

	/** Called by the player's health component death delegate. */
	UFUNCTION()
	void HandlePlayerDied(AActor* Killer);

	/** Override in a BP subclass to reset inventory / world items (§10.1 step 7). */
	UFUNCTION(BlueprintNativeEvent, Category="Game")
	void ResetPlayerAndWorld(APawn* PlayerPawn);
	virtual void ResetPlayerAndWorld_Implementation(APawn* PlayerPawn);

	private:
	UPROPERTY()
	FVector CachedPlayerStartLocation = FVector::ZeroVector;

	UPROPERTY()
	FRotator CachedPlayerStartRotation = FRotator::ZeroRotator;
};