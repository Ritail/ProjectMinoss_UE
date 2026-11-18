// BTTask_LookAround.cpp
#include "BTTask_LookAround.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_LookAround::UBTTask_LookAround()
{
    NodeName = TEXT("Look Around");
    bNotifyTick = true;
    bNotifyTaskFinished = false;
    bCreateNodeInstance = false;
}

uint16 UBTTask_LookAround::GetInstanceMemorySize() const
{
    return sizeof(FBTLookAroundMemory);
}

EBTNodeResult::Type UBTTask_LookAround::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
    if (!Pawn) return EBTNodeResult::Failed;

    FBTLookAroundMemory* Memory = CastInstanceNodeMemory<FBTLookAroundMemory>(NodeMemory);
    Memory->StartRotation = Pawn->GetActorRotation();
    Memory->ElapsedTime   = 0.f;
    Memory->Duration      = FMath::Max(0.1f, Duration);

    return EBTNodeResult::InProgress;
}

void UBTTask_LookAround::TickTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
    if (!Pawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    FBTLookAroundMemory* Memory = CastInstanceNodeMemory<FBTLookAroundMemory>(NodeMemory);
    Memory->ElapsedTime += DeltaSeconds;

    if (Memory->ElapsedTime >= Memory->Duration)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    const float Alpha     = Memory->ElapsedTime / Memory->Duration;
    const float YawOffset = SweepAngleDegrees * FMath::Sin(2.f * PI * Alpha);

    Pawn->SetActorRotation(FRotator(
        Memory->StartRotation.Pitch,
        Memory->StartRotation.Yaw + YawOffset,
        Memory->StartRotation.Roll));
}

FString UBTTask_LookAround::GetStaticDescription() const
{
    return FString::Printf(TEXT("Sweep ±%.0f° over %.1fs"), SweepAngleDegrees, Duration);
}