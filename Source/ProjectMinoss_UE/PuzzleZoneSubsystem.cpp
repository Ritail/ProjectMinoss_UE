// PuzzleZoneSubsystem.cpp
#include "PuzzleZoneSubsystem.h"
#include "EnemyGameSettings.h"

bool UPuzzleZoneSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer)) return false;
    const UWorld* World = Cast<UWorld>(Outer);
    return World && World->IsGameWorld();
}

void UPuzzleZoneSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // Source of truth for total count. No BeginPlay registration race.
    TotalZoneCount = UEnemyGameSettings::Get()->ZoneIds.Num();
    CompletedZones.Reset();
}

void UPuzzleZoneSubsystem::Deinitialize()
{
    CompletedZones.Reset();
    TotalZoneCount = 0;
    Super::Deinitialize();
}

void UPuzzleZoneSubsystem::MarkZoneCompleted(FGameplayTag ZoneId)
{
    if (!ZoneId.IsValid()) return;
    if (CompletedZones.Contains(ZoneId)) return;   // idempotent

    CompletedZones.Add(ZoneId);
    OnZoneCompleted.Broadcast(ZoneId);
}

bool UPuzzleZoneSubsystem::IsZoneCompleted(FGameplayTag ZoneId) const
{
    return CompletedZones.Contains(ZoneId);
}

int32 UPuzzleZoneSubsystem::GetCompletedCount() const
{
    return CompletedZones.Num();
}

void UPuzzleZoneSubsystem::Reset()
{
    CompletedZones.Reset();
    OnZonesReset.Broadcast();
    // Note: OnGameReset is broadcast by AEnemyGameMode, AFTER this runs.
}