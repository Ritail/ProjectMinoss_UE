// HealthComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDied,    AActor*, Killer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRevived);

UCLASS(ClassGroup=(Player), meta=(BlueprintSpawnableComponent))
class PROJECTMINOSS_UE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(EditAnywhere, Category="Health", meta=(ClampMin="1.0"))
	float MaxHealth = 100.f;

	UFUNCTION(BlueprintCallable, Category="Health")
	void Die(AActor* Killer = nullptr);

	/** Called by GameMode reset path. Clears bDead and broadcasts OnRevived. */
	UFUNCTION(BlueprintCallable, Category="Health")
	void Revive();

	UFUNCTION(BlueprintPure, Category="Health")
	bool IsDead() const { return bDead; }

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDied OnDied;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnRevived OnRevived;

private:
	bool bDead = false;
};