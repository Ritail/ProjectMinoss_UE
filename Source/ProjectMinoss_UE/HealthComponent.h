// HealthComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDied, AActor*, Killer);

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

	UFUNCTION(BlueprintPure, Category="Health")
	bool IsDead() const { return bDead; }

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDied OnDied;

	private:
	bool bDead = false;
};