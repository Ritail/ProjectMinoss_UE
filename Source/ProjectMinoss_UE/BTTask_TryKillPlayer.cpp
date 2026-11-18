// BTTask_TryKillPlayer.cpp
#include "BTTask_TryKillPlayer.h"
#include "AIController.h"
#include "EnemyCharacter.h"
#include "Killable.h"
#include "AIBlackboardKeys.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"

UBTTask_TryKillPlayer::UBTTask_TryKillPlayer()
{
    NodeName = TEXT("Try Kill Player");
    bNotifyTaskFinished = false;
    bCreateNodeInstance = true;   // per-AI instance so CachedOwnerComp is safe
}

EBTNodeResult::Type UBTTask_TryKillPlayer::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    CachedOwnerComp = &OwnerComp;

    AAIController* AIC = OwnerComp.GetAIOwner();
    AEnemyCharacter* Enemy = AIC ? Cast<AEnemyCharacter>(AIC->GetPawn()) : nullptr;
    if (!Enemy) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(BBKeys::TargetActor)) : nullptr;
    if (!Target) return EBTNodeResult::Failed;

    // Range check (first).
    const float DistSq = FVector::DistSquared(Enemy->GetActorLocation(), Target->GetActorLocation());
    if (DistSq > FMath::Square(KillRange)) return EBTNodeResult::Failed;

    CachedTarget = Target;

    // Fast path — no montage, no anim instance. Kill immediately.
    UAnimInstance* AnimInst = Enemy->GetMesh() ? Enemy->GetMesh()->GetAnimInstance() : nullptr;
    if (!KillMontage || !AnimInst)
    {
        ApplyKill();
        CachedOwnerComp = nullptr;
        return EBTNodeResult::Succeeded;
    }

    // Montage path.
    CachedAnimInstance = AnimInst;
    if (!AnimInst->OnMontageEnded.IsAlreadyBound(this, &UBTTask_TryKillPlayer::OnKillMontageEnded))
    {
        AnimInst->OnMontageEnded.AddDynamic(this, &UBTTask_TryKillPlayer::OnKillMontageEnded);
    }
    Enemy->PlayAnimMontage(KillMontage);
    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_TryKillPlayer::AbortTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UnbindMontageDelegate();
    CachedOwnerComp = nullptr;
    CachedTarget.Reset();
    return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_TryKillPlayer::OnKillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != KillMontage) return;   // ignore unrelated montages
    if (bInterrupted) { UnbindMontageDelegate(); return; }

    ApplyKill();
    UnbindMontageDelegate();

    if (CachedOwnerComp)
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
    }
    CachedOwnerComp = nullptr;
    CachedTarget.Reset();
}

void UBTTask_TryKillPlayer::ApplyKill()
{
    AActor* Target = CachedTarget.Get();
    if (!Target) return;

    if (bRecheckRangeAfterMontage && CachedOwnerComp)
    {
        AAIController* AIC = CachedOwnerComp->GetAIOwner();
        APawn* EnemyPawn = AIC ? AIC->GetPawn() : nullptr;
        if (!EnemyPawn) return;

        const float DistSq = FVector::DistSquared(EnemyPawn->GetActorLocation(), Target->GetActorLocation());
        if (DistSq > FMath::Square(KillRange)) return;   // player dodged during windup
    }

    if (IKillable* Killable = Cast<IKillable>(Target))
    {
        AAIController* AIC = CachedOwnerComp ? CachedOwnerComp->GetAIOwner() : nullptr;
        Killable->Kill(AIC ? AIC->GetPawn() : nullptr);
    }
}

void UBTTask_TryKillPlayer::UnbindMontageDelegate()
{
    if (CachedAnimInstance)
    {
        CachedAnimInstance->OnMontageEnded.RemoveDynamic(this, &UBTTask_TryKillPlayer::OnKillMontageEnded);
        CachedAnimInstance = nullptr;
    }
}

FString UBTTask_TryKillPlayer::GetStaticDescription() const
{
    return FString::Printf(TEXT("Kill in %.0f units%s"),
        KillRange,
        KillMontage ? TEXT(" (with montage)") : TEXT(" (instant)"));
}