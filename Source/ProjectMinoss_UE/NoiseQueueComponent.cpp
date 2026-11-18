// NoiseQueueComponent.cpp
#include "NoiseQueueComponent.h"
#include "EnemyGameSettings.h"

UNoiseQueueComponent::UNoiseQueueComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.25f; // coarse — only for purge-on-expiry
}

void UNoiseQueueComponent::BeginPlay()
{
    Super::BeginPlay();

    const UEnemyGameSettings* Settings = UEnemyGameSettings::Get();
    Queue.Configure(Settings->NoiseThreshold, Settings->NoiseWindowSeconds);
}

void UNoiseQueueComponent::PushNoise(const FVector& Location, AActor* Instigator)
{
    UWorld* World = GetWorld();
    if (!World) return;

    const double Now = World->GetTimeSeconds();

    FNoiseRecord Record;
    Record.Location   = Location;
    Record.Instigator = Instigator;
    Record.TimeStamp  = Now;

    Queue.Push(Record, Now);

    // Latest noise becomes the active investigate point.
    if (const FNoiseRecord* Latest = Queue.Latest())
    {
        OnInvestigateLocationChanged.Broadcast(Latest->Location, true);
    }

    EvaluateTrigger(Instigator);
}

void UNoiseQueueComponent::EvaluateTrigger(AActor* LatestInstigator)
{
    if (!Queue.ShouldTrigger()) return;

    // Resolve the instigator from the most recent record if the caller didn't pass one.
    if (!LatestInstigator)
    {
        if (const FNoiseRecord* Latest = Queue.Latest())
        {
            LatestInstigator = Latest->Instigator.Get();
        }
    }

    // Clear before broadcast so re-entry is impossible while overload is active.
    Queue.Clear();
    bOverloaded = true;
    OnNoiseOverload.Broadcast(LatestInstigator);

    // Note: bOverloaded is reset when the controller removes the SoundOverload status.
    // Controller calls Clear() at that point.
}

void UNoiseQueueComponent::Clear()
{
    Queue.Clear();
    bOverloaded = false;
    OnInvestigateLocationChanged.Broadcast(FVector::ZeroVector, false);
}

void UNoiseQueueComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UWorld* World = GetWorld();
    if (!World || Queue.Num() == 0) return;

    const int32 Before = Queue.Num();
    Queue.PurgeExpired(World->GetTimeSeconds());

    if (Queue.Num() == 0 && Before > 0)
    {
        // Last record aged out — clear the investigate point so stale noise can't
        // pull the enemy out of patrol.
        OnInvestigateLocationChanged.Broadcast(FVector::ZeroVector, false);
    }
    else if (const FNoiseRecord* Latest = Queue.Latest())
    {
        // Latest may have changed due to purge — keep InvestigateLocation fresh.
        OnInvestigateLocationChanged.Broadcast(Latest->Location, true);
    }
}