// NoiseQueue.h
#pragma once

#include "CoreMinimal.h"

class AActor;

struct FNoiseRecord
{
	FVector Location = FVector::ZeroVector;
	TWeakObjectPtr<AActor> Instigator;
	double TimeStamp = 0.0;
};

/**
 * Pure logic. No UObjects, no timers, no world. Unit-testable.
 * Owned by UNoiseQueueComponent; driven by Push/Purge calls.
 */
struct FNoiseQueue
{
	void Configure(int32 InThreshold, double InWindowSeconds);
	void Push(const FNoiseRecord& Record, double Now);
	void PurgeExpired(double Now);

	bool  ShouldTrigger() const   { return Records.Num() >= Threshold; }
	int32 Num() const             { return Records.Num(); }
	const FNoiseRecord* Latest() const { return Records.Num() > 0 ? &Records.Last() : nullptr; }
	void  Clear()                 { Records.Reset(); }

private:
	TArray<FNoiseRecord> Records;
	int32  Threshold     = 3;
	double WindowSeconds = 4.0;
};
