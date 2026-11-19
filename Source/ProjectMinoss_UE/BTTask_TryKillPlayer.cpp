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
    bCreateNodeInstance = true;   // per-AI instance so the cached pointers are safe
}

EBTNodeResult::Type UBTTask_TryKillPlayer::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    AEnemyCharacter* Enemy = AIC ? Cast<AEnemyCharacter>(AIC->GetPawn()) : nullptr;
    if (!Enemy) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(BBKeys::TargetActor)) : nullptr;
    if (!Target || !Cast<IKillable>(Target)) return EBTNodeResult::Failed;

    if (!IsInKillRange(Enemy, Target)) return EBTNodeResult::Failed;

    CachedOwnerComp = &OwnerComp;
    CachedTarget = Target;

    // No montage (or no anim instance to play it on): the kill lands now.
    UAnimInstance* AnimInst = Enemy->GetMesh() ? Enemy->GetMesh()->GetAnimInstance() : nullptr;
    if (!KillMontage || !AnimInst)
    {
        ExecuteKill(Target, Enemy);
        ResetCache();
        return EBTNodeResult::Succeeded;
    }

    // Montage path: wait for the end event.
    CachedAnimInstance = AnimInst;
    if (!AnimInst->OnMontageEnded.IsAlreadyBound(this, &UBTTask_TryKillPlayer::OnKillMontageEnded))
    {
        AnimInst->OnMontageEnded.AddDynamic(this, &UBTTask_TryKillPlayer::OnKillMontageEnded);
    }

    // If the montage cannot start, no end event will ever arrive. Resolve now instead of hanging.
    if (Enemy->PlayAnimMontage(KillMontage) <= 0.f)
    {
        UnbindMontageDelegate();
        ExecuteKill(Target, Enemy);
        ResetCache();
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_TryKillPlayer::AbortTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Unbind first, so stopping the montage below cannot finish this task.
    UnbindMontageDelegate();

    if (KillMontage)
    {
        if (AAIController* AIC = OwnerComp.GetAIOwner())
        {
            if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIC->GetPawn()))
            {
                UAnimInstance* Anim = Enemy->GetMesh() ? Enemy->GetMesh()->GetAnimInstance() : nullptr;
                if (Anim && Anim->Montage_IsPlaying(KillMontage))
                {
                    Enemy->StopAnimMontage(KillMontage);
                }
            }
        }
    }

    ResetCache();
    return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_TryKillPlayer::OnKillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != KillMontage) return;   // ignore unrelated montages
    UnbindMontageDelegate();

    UBehaviorTreeComponent* OwnerComp = CachedOwnerComp;
    AActor* Target = CachedTarget.Get();
    APawn* EnemyPawn = (OwnerComp && OwnerComp->GetAIOwner()) ? OwnerComp->GetAIOwner()->GetPawn() : nullptr;

    // An interrupted montage or a dodged kill is a failure, not a success.
    const bool bCanKill =
        !bInterrupted &&
        Target != nullptr &&
        EnemyPawn != nullptr &&
        (!bRecheckRangeAfterMontage || IsInKillRange(EnemyPawn, Target));

    ResetCache();

    // Finish before killing. The kill may reset the world and stop this tree.
    if (OwnerComp)
    {
        FinishLatentTask(*OwnerComp, bCanKill ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
    }

    if (bCanKill)
    {
        ExecuteKill(Target, EnemyPawn);
    }
}

bool UBTTask_TryKillPlayer::IsInKillRange(const APawn* EnemyPawn, const AActor* Target) const
{
    if (!EnemyPawn || !Target) return false;
    return FVector::DistSquared(EnemyPawn->GetActorLocation(), Target->GetActorLocation())
        <= FMath::Square(KillRange);
}

void UBTTask_TryKillPlayer::ExecuteKill(AActor* Target, APawn* EnemyPawn)
{
    if (Target && Cast<IKillable>(Target))
    {
        IKillable::Execute_Kill(Target, EnemyPawn);
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

void UBTTask_TryKillPlayer::ResetCache()
{
    CachedOwnerComp = nullptr;
    CachedTarget.Reset();
}

FString UBTTask_TryKillPlayer::GetStaticDescription() const
{
    return FString::Printf(TEXT("Kill in %.0f units%s"),
        KillRange,
        KillMontage ? TEXT(" (with montage)") : TEXT(" (instant)"));
}
