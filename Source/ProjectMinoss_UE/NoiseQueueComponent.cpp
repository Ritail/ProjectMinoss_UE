// NoiseQueueComponent.cpp
#include "NoiseQueueComponent.h"
#include "EnemyGameSettings.h"

UNoiseQueueComponent::UNoiseQueueComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.25f; // coarse, only for purge-on-expiry
}

void UNoiseQueueComponent::BeginPlay()
{
    Super::BeginPlay();

    if (const UEnemyGameSettings* Settings = UEnemyGameSettings::Get())
    {
        Queue.Configure(Settings->NoiseThreshold, Settings->NoiseWindowSeconds);
    }
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

    // The latest noise becomes the active investigate point.
    if (const FNoiseRecord* Latest = Queue.Latest())
    {
        BroadcastInvestigate(true, Latest->Location);
    }

    EvaluateTrigger(Instigator);
}

void UNoiseQueueComponent::EvaluateTrigger(AActor* LatestInstigator)
{
    if (!Queue.ShouldTrigger()) return;

    // Resolve the instigator from the newest record if the caller passed none.
    if (!LatestInstigator)
    {
        if (const FNoiseRecord* Latest = Queue.Latest())
        {
            LatestInstigator = Latest->Instigator.Get();
        }
    }

    // Clear before broadcasting, so re-entry cannot retrigger. The queue's
    // records are consumed by the overload, so the investigate point goes too.
    // Otherwise the enemy would walk to that noise after the chase ends.
    Queue.Clear();
    bOverloaded = true;
    BroadcastInvestigate(false, FVector::ZeroVector);
    OnNoiseOverload.Broadcast(LatestInstigator);
}

void UNoiseQueueComponent::Clear()
{
    Queue.Clear();
    bOverloaded = false;
    BroadcastInvestigate(false, FVector::ZeroVector);
}

void UNoiseQueueComponent::ClearOverloadFlag()
{
    bOverloaded = false;
}

void UNoiseQueueComponent::BroadcastInvestigate(bool bValid, const FVector& Location)
{
    if (bValid == bLastInvestigateValid && (!bValid || Location.Equals(LastInvestigateLocation)))
    {
        return;   // no change, so no Blackboard write
    }

    bLastInvestigateValid = bValid;
    LastInvestigateLocation = bValid ? Location : FVector::ZeroVector;
    OnInvestigateLocationChanged.Broadcast(LastInvestigateLocation, bValid);
}

void UNoiseQueueComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UWorld* World = GetWorld();
    if (!World || Queue.Num() == 0) return;

    Queue.PurgeExpired(World->GetTimeSeconds());

    // Purging may empty the queue. This check runs after the purge, so an empty
    // queue invalidates the investigate point here.
    if (const FNoiseRecord* Latest = Queue.Latest())
    {
        BroadcastInvestigate(true, Latest->Location);
    }
    else
    {
        BroadcastInvestigate(false, FVector::ZeroVector);
    }
}
