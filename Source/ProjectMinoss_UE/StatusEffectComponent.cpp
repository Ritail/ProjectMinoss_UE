// StatusEffectComponent.cpp
#include "StatusEffectComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UStatusEffectComponent::UStatusEffectComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UStatusEffectComponent::ApplyStatus(
    FGameplayTag Tag, float Duration, EStatusStackPolicy Policy, UObject* Instigator)
{
    if (!Tag.IsValid()) return;
    UWorld* World = GetWorld();
    if (!World) return;
    FTimerManager& TM = World->GetTimerManager();

    const bool bWasActive = ActiveStatuses.Contains(Tag);

    // Ignore policy is only a no-op when the status is already active.
    if (bWasActive && Policy == EStatusStackPolicy::Ignore) return;

    FStatusInstance& Instance = ActiveStatuses.FindOrAdd(Tag);
    Instance.Generation  = ++GenerationCounter;
    Instance.Duration    = Duration;

    if (Duration <= 0.f)
    {
        // Infinite — no timer.
        TM.ClearTimer(Instance.Timer);
    }
    else
    {
        float StartDuration = Duration;

        // Extend only meaningful if there's a running timer to add to.
        if (bWasActive && TM.IsTimerActive(Instance.Timer) && Policy == EStatusStackPolicy::Extend)
        {
            const float Current = TM.GetTimerRemaining(Instance.Timer);
            if (Current > 0.f) StartDuration = Current + Duration;
        }
        // Refresh (or fresh application) falls through with StartDuration = Duration.

        TM.ClearTimer(Instance.Timer);

        // Capture this instance's generation. If a later ApplyStatus bumps
        // GenerationCounter, this stale timer will no-op when it fires.
        const uint32 CapturedGen = Instance.Generation;
        FTimerDelegate Del = FTimerDelegate::CreateUObject(
            this, &UStatusEffectComponent::HandleExpiry, Tag, CapturedGen);

        TM.SetTimer(Instance.Timer, Del, StartDuration, /*bLoop=*/false);
    }

    OnStatusAdded.Broadcast(Tag, Instigator);
}

void UStatusEffectComponent::HandleExpiry(FGameplayTag Tag, uint32 Generation)
{
    FStatusInstance* Instance = ActiveStatuses.Find(Tag);
    if (!Instance || Instance->Generation != Generation)
    {
        return; // a newer application superseded this timer
    }
    ActiveStatuses.Remove(Tag);
    OnStatusRemoved.Broadcast(Tag, EStatusRemovalReason::Expired);
}

void UStatusEffectComponent::RemoveStatus(FGameplayTag Tag, EStatusRemovalReason Reason)
{
    FStatusInstance* Instance = ActiveStatuses.Find(Tag);
    if (!Instance) return;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(Instance->Timer);
    }
    ActiveStatuses.Remove(Tag);
    OnStatusRemoved.Broadcast(Tag, Reason);
}

void UStatusEffectComponent::ClearAll(EStatusRemovalReason Reason)
{
    // Snapshot keys to avoid mutating the map during iteration.
    TArray<FGameplayTag> Keys;
    ActiveStatuses.GetKeys(Keys);
    for (const FGameplayTag& Tag : Keys)
    {
        RemoveStatus(Tag, Reason);
    }
}