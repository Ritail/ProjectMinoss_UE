// NoiseQueue.cpp
#include "NoiseQueue.h"

void FNoiseQueue::Configure(int32 InThreshold, double InWindowSeconds)
{
	Threshold     = FMath::Max(1, InThreshold);
	WindowSeconds = FMath::Max(0.1, InWindowSeconds);
}

void FNoiseQueue::Push(const FNoiseRecord& Record, double Now)
{
	Records.Add(Record);
	PurgeExpired(Now);
}

void FNoiseQueue::PurgeExpired(double Now)
{
	const double Cutoff = Now - WindowSeconds;
	Records.RemoveAll([Cutoff](const FNoiseRecord& R) { return R.TimeStamp < Cutoff; });
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNoiseQueueTest, "Enemy.NoiseQueue", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FNoiseQueueTest::RunTest(const FString&)
{
	FNoiseQueue Q;
	Q.Configure(3, 4.0);
	Q.Push({FVector::ZeroVector, nullptr, 0.0}, 0.0);
	Q.Push({FVector::ZeroVector, nullptr, 1.0}, 1.0);
	TestFalse(TEXT("Two noises should not trigger"), Q.ShouldTrigger());
	Q.Push({FVector::ZeroVector, nullptr, 2.0}, 2.0);
	TestTrue(TEXT("Three noises should trigger"), Q.ShouldTrigger());
	Q.PurgeExpired(10.0);
	TestEqual(TEXT("All should expire at t=10"), Q.Num(), 0);
	return true;
}