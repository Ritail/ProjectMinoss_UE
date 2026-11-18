// StatusEffectComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "StatusEffectComponent.generated.h"

UENUM(BlueprintType)
enum class EStatusStackPolicy : uint8
{
	Extend  UMETA(DisplayName="Extend (add to remaining)"),
	Refresh UMETA(DisplayName="Refresh (reset to full)"),
	Ignore  UMETA(DisplayName="Ignore (no-op if active)")
};

UENUM(BlueprintType)
enum class EStatusRemovalReason : uint8
{
	Expired,
	Cleared,
	Reset,
	Superseded
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatusAdded, FGameplayTag /*Tag*/, UObject* /*Instigator*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatusRemoved, FGameplayTag /*Tag*/, EStatusRemovalReason /*Reason*/);

UCLASS(ClassGroup=(Enemy), meta=(BlueprintSpawnableComponent))
class PROJECTMINOSS_UE_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

	public:
	UStatusEffectComponent();

	UFUNCTION(BlueprintCallable, Category="Status")
	void ApplyStatus(FGameplayTag Tag, float Duration, EStatusStackPolicy Policy, UObject* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, Category="Status")
	void RemoveStatus(FGameplayTag Tag, EStatusRemovalReason Reason = EStatusRemovalReason::Cleared);

	UFUNCTION(BlueprintPure, Category="Status")
	bool HasStatus(FGameplayTag Tag) const { return ActiveStatuses.Contains(Tag); }

	UFUNCTION(BlueprintCallable, Category="Status")
	void ClearAll(EStatusRemovalReason Reason = EStatusRemovalReason::Cleared);

	FOnStatusAdded   OnStatusAdded;
	FOnStatusRemoved OnStatusRemoved;

	private:
	struct FStatusInstance
	{
		float    Duration = 0.f;    // 0 == infinite
		uint32   Generation = 0;    // guards against stale-timer races
		FTimerHandle Timer;
	};

	TMap<FGameplayTag, FStatusInstance> ActiveStatuses;
	uint32 GenerationCounter = 0;

	void HandleExpiry(FGameplayTag Tag, uint32 Generation);
};