// NoiseQueue.cpp
#include "NoiseQueue.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#endif

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

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FNoiseQueueTest,
    "ProjectMinoss.NoiseQueue",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNoiseQueueTest::RunTest(const FString& /*Parameters*/)
{
    using FQ = FNoiseQueue;

    // --- Test 1: threshold not met under the configured count ---
    {
        FQ Q;
        Q.Configure(/*Threshold=*/3, /*Window=*/4.0);
        Q.Push({FVector::ZeroVector, nullptr, 0.0}, 0.0);
        Q.Push({FVector::ZeroVector, nullptr, 1.0}, 1.0);
        TestFalse(TEXT("2 of 3 noises should not trigger"), Q.ShouldTrigger());
        TestEqual(TEXT("queue should hold 2 records"), Q.Num(), 2);
    }

    // --- Test 2: threshold met ---
    {
        FQ Q;
        Q.Configure(3, 4.0);
        Q.Push({FVector::ZeroVector, nullptr, 0.0}, 0.0);
        Q.Push({FVector::ZeroVector, nullptr, 1.0}, 1.0);
        Q.Push({FVector::ZeroVector, nullptr, 2.0}, 2.0);
        TestTrue(TEXT("3 of 3 noises should trigger"), Q.ShouldTrigger());
    }

    // --- Test 3: sliding window expires old records ---
    {
        FQ Q;
        Q.Configure(3, 4.0);
        Q.Push({FVector::ZeroVector, nullptr, 0.0}, 0.0);
        Q.Push({FVector::ZeroVector, nullptr, 1.0}, 1.0);
        // Advance to t=6.0 — both records are now well past the 4s window.
        Q.PurgeExpired(6.0);
        TestEqual(TEXT("both records should have expired"), Q.Num(), 0);
        TestFalse(TEXT("no records → should not trigger"), Q.ShouldTrigger());
    }

    // --- Test 3b: boundary semantics — record at exactly Now-Window survives ---
    {
        FQ Q;
        Q.Configure(3, 4.0);
        Q.Push({FVector::ZeroVector, nullptr, 1.0}, 1.0);
        // At Now=5.0, the record is exactly 4.0s old — the boundary is inclusive.
        Q.PurgeExpired(5.0);
        TestEqual(TEXT("record exactly at window boundary survives"), Q.Num(), 1);
    }

    // --- Test 4: newest survives, oldest purged ---
    {
        FQ Q;
        Q.Configure(3, 4.0);
        Q.Push({FVector::ZeroVector, nullptr, 0.0}, 0.0);
        Q.Push({FVector::ZeroVector, nullptr, 3.0}, 3.0);
        Q.PurgeExpired(4.5);  // t=0 expired (>4s old), t=3 alive (1.5s old)
        TestEqual(TEXT("only the t=3 record should remain"), Q.Num(), 1);
        const FNoiseRecord* Latest = Q.Latest();
        TestNotNull(TEXT("Latest should return the surviving record"), Latest);
        if (Latest) TestEqual(TEXT("Latest timestamp should be 3.0"), Latest->TimeStamp, 3.0);
    }

    // --- Test 5: Latest() returns the newest by insertion order ---
    {
        FQ Q;
        Q.Configure(3, 4.0);
        Q.Push({FVector(1,0,0), nullptr, 0.0}, 0.0);
        Q.Push({FVector(2,0,0), nullptr, 1.0}, 1.0);
        const FNoiseRecord* Latest = Q.Latest();
        TestNotNull(TEXT("Latest should be non-null"), Latest);
        if (Latest) TestTrue(TEXT("Latest should be the second push"), Latest->Location.Equals(FVector(2,0,0)));
    }

    // --- Test 6: Clear wipes everything ---
    {
        FQ Q;
        Q.Configure(3, 4.0);
        Q.Push({FVector::ZeroVector, nullptr, 0.0}, 0.0);
        Q.Push({FVector::ZeroVector, nullptr, 1.0}, 1.0);
        Q.Clear();
        TestEqual(TEXT("Clear should empty the queue"), Q.Num(), 0);
        TestFalse(TEXT("Clear → should not trigger"), Q.ShouldTrigger());
    }

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
