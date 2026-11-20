// EnemyAIController.cpp
#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "StatusEffectComponent.h"
#include "NoiseQueueComponent.h"
#include "EnemyGameplayTags.h"
#include "EnemyGameSettings.h"
#include "PuzzleZoneSubsystem.h"
#include "AIBlackboardKeys.h"
#include "BlackboardKeyType_GameplayTag.h"
#include "Killable.h"
#include "PatrolRouteComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyAIController::AEnemyAIController()
{
    UAIPerceptionComponent* Perc = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    SetPerceptionComponent(*Perc);

    NoiseQueueComp = CreateDefaultSubobject<UNoiseQueueComponent>(TEXT("NoiseQueue"));

    UAISenseConfig_Sight* Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    // Neutrals stay detected on purpose. A pawn with no team ID is NoTeam, which
    // the perception system treats as neutral, so turning this off would also
    // hide the player. Non-killable neutrals are filtered in the perception handler.
    Sight->DetectionByAffiliation.bDetectEnemies    = true;
    Sight->DetectionByAffiliation.bDetectNeutrals   = true;
    Sight->DetectionByAffiliation.bDetectFriendlies = false;
    Sight->LoseSightRadius              = 1800.f;
    Sight->PeripheralVisionAngleDegrees = 35.f;   // half-angle
    Sight->AutoSuccessRangeFromLastSeenLocation = 100.f;
    Perc->ConfigureSense(*Sight);

    UAISenseConfig_Hearing* Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    Hearing->HearingRange = 2500.f;
    Hearing->DetectionByAffiliation.bDetectEnemies    = true;
    Hearing->DetectionByAffiliation.bDetectNeutrals   = true;
    Hearing->DetectionByAffiliation.bDetectFriendlies = false;
    Perc->ConfigureSense(*Hearing);

    Perc->SetDominantSense(Sight->GetSenseImplementation());

    // Baseline sight radius (settings override in OnPossess)
    BaseSightRadius = Sight->SightRadius;
}

// --- Possession ---

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);
    if (!Enemy) return;

    ConfigurePerception();

    // 1. Blackboard + tree.
    if (!ensureMsgf(BlackboardAsset, TEXT("BlackboardAsset missing on %s"), *GetName()))
    {
        return;
    }
    UBlackboardComponent* BBComp = nullptr;
    if (!UseBlackboard(BlackboardAsset, BBComp))
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyAIController: UseBlackboard failed"));
        return;
    }
    if (ensureMsgf(BehaviorTreeAsset, TEXT("BehaviorTreeAsset missing on %s"), *GetName()))
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }

    // 2. Blackboard defaults. These are written after RunBehaviorTree (guide 5.1).
    bCanSeeTarget = false;
    BBComp->SetValueAsVector(BBKeys::HomeLocation, Enemy->GetHomeLocation());
    BBComp->ClearValue(BBKeys::TargetActor);
    BBComp->ClearValue(BBKeys::LastKnownLocation);
    BBComp->ClearValue(BBKeys::InvestigateLocation);
    BBComp->ClearValue(BBKeys::CurrentPatrolPoint);
    BBComp->SetValue<UBlackboardKeyType_GameplayTag>(BBKeys::StateTag, EnemyTags::State_Patrol);

    // 3. Listeners.
    BindPawnDelegates();

    // 4. Notify character.
    Enemy->OnAIPossessed(this);
}

void AEnemyAIController::OnUnPossess()
{
    UnbindPawnDelegates();
    Super::OnUnPossess();
}

void AEnemyAIController::EndPlay(const EEndPlayReason::Type Reason)
{
    UnbindPawnDelegates();
    Super::EndPlay(Reason);
}

void AEnemyAIController::BindPawnDelegates()
{
    // Idempotent: clear any previous binding before adding new ones.
    UnbindPawnDelegates();

    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
        {
            Status->OnStatusAdded.AddUObject(this, &AEnemyAIController::HandleStatusAdded);
            Status->OnStatusRemoved.AddUObject(this, &AEnemyAIController::HandleStatusRemoved);
        }
    }

    if (NoiseQueueComp)
    {
        NoiseQueueComp->OnNoiseOverload.AddUObject(this, &AEnemyAIController::HandleNoiseOverload);
        NoiseQueueComp->OnInvestigateLocationChanged.AddUObject(this, &AEnemyAIController::HandleInvestigateLocationChanged);
    }

    if (UWorld* World = GetWorld())
    {
        if (UPuzzleZoneSubsystem* Zones = World->GetSubsystem<UPuzzleZoneSubsystem>())
        {
            Zones->OnZoneCompleted.AddUniqueDynamic(this, &AEnemyAIController::HandleZoneCompleted);
        }
    }

    if (UAIPerceptionComponent* Perc = GetPerceptionComponent())
    {
        Perc->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AEnemyAIController::HandleTargetPerceptionUpdated);
    }
}

void AEnemyAIController::UnbindPawnDelegates()
{
    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
        {
            Status->OnStatusAdded.RemoveAll(this);
            Status->OnStatusRemoved.RemoveAll(this);
        }
    }

    if (NoiseQueueComp)
    {
        NoiseQueueComp->OnNoiseOverload.RemoveAll(this);
        NoiseQueueComp->OnInvestigateLocationChanged.RemoveAll(this);
    }

    if (UWorld* World = GetWorld())
    {
        if (UPuzzleZoneSubsystem* Zones = World->GetSubsystem<UPuzzleZoneSubsystem>())
        {
            Zones->OnZoneCompleted.RemoveAll(this);
        }
    }

    if (UAIPerceptionComponent* Perc = GetPerceptionComponent())
    {
        Perc->OnTargetPerceptionUpdated.RemoveAll(this);
    }
}

// --- Accessors and shared state ---

AEnemyCharacter* AEnemyAIController::GetEnemyCharacter() const
{
    return Cast<AEnemyCharacter>(GetPawn());
}

UBehaviorTreeComponent* AEnemyAIController::GetBTComponent() const
{
    return Cast<UBehaviorTreeComponent>(BrainComponent);
}

AActor* AEnemyAIController::GetTargetActor() const
{
    const UBlackboardComponent* BB = GetBlackboardComponent();
    return BB ? Cast<AActor>(BB->GetValueAsObject(BBKeys::TargetActor)) : nullptr;
}

bool AEnemyAIController::HasSoundOverload() const
{
    const AEnemyCharacter* Enemy = GetEnemyCharacter();
    const UStatusEffectComponent* Status = Enemy ? Enemy->GetStatusComponent() : nullptr;
    return Status && Status->HasStatus(EnemyTags::Status_SoundOverload);
}

bool AEnemyAIController::IsPursuing() const
{
    return GetTargetActor() != nullptr && (bCanSeeTarget || HasSoundOverload());
}

void AEnemyAIController::SetAIState(FGameplayTag NewState)
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValue<UBlackboardKeyType_GameplayTag>(BBKeys::StateTag, NewState);
    }
    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        Enemy->RecalculateMovementStats();
    }
}

void AEnemyAIController::SetTargetActor(AActor* Target)
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;

    if (Target)
    {
        BB->SetValueAsObject(BBKeys::TargetActor, Target);
    }
    else
    {
        BB->ClearValue(BBKeys::TargetActor);
    }
}

void AEnemyAIController::ConfigurePerception()
{
    const UEnemyGameSettings* Settings = UEnemyGameSettings::Get();
    UAIPerceptionComponent* Perc = GetPerceptionComponent();
    if (!Settings || !Perc) return;

    if (UAISenseConfig_Sight* Sight = Cast<UAISenseConfig_Sight>(
        Perc->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>())))
    {
        Sight->SightRadius                  = Settings->EnemySightRadius;
        Sight->LoseSightRadius              = Settings->EnemySightRadius * 1.2f;
        Sight->PeripheralVisionAngleDegrees = Settings->EnemySightFOVDegrees * 0.5f;
        BaseSightRadius = Sight->SightRadius;
        Perc->ConfigureSense(*Sight);
    }

    if (UAISenseConfig_Hearing* Hearing = Cast<UAISenseConfig_Hearing>(
        Perc->GetSenseConfig(UAISense::GetSenseID<UAISense_Hearing>())))
    {
        Hearing->HearingRange = Settings->EnemyHearingRange;
        Perc->ConfigureSense(*Hearing);
    }

    Perc->RequestStimuliListenerUpdate();
}

void AEnemyAIController::SetSightEnabled(bool bEnabled)
{
    if (UAIPerceptionComponent* Perc = GetPerceptionComponent())
    {
        Perc->SetSenseEnabled(UAISense_Sight::StaticClass(), bEnabled);
    }
}

void AEnemyAIController::RecalculateSightRange()
{
    UAIPerceptionComponent* Perc = GetPerceptionComponent();
    if (!Perc) return;

    UAISenseConfig_Sight* Sight = Cast<UAISenseConfig_Sight>(
        Perc->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));
    if (!Sight) return;

    const UEnemyGameSettings* Settings = UEnemyGameSettings::Get();
    float Radius = BaseSightRadius;
    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
        {
            if (Status->HasStatus(EnemyTags::Status_Rage) && Settings)
            {
                Radius *= Settings->RageSightMultiplier;
            }
        }
    }
    Sight->SightRadius     = Radius;
    Sight->LoseSightRadius = Radius * 1.2f;
    Perc->ConfigureSense(*Sight);
    Perc->RequestStimuliListenerUpdate();
}

// --- Perception ---

void AEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    const FAISenseID SightID   = UAISense::GetSenseID<UAISense_Sight>();
    const FAISenseID HearingID = UAISense::GetSenseID<UAISense_Hearing>();

    if (Stimulus.Type == SightID)
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            // Only chase things we can kill. Neutral props and NPCs are ignored.
            if (!Cast<IKillable>(Actor)) return;

            bCanSeeTarget = true;
            SetTargetActor(Actor);
        }
        else
        {
            bCanSeeTarget = false;

            // Only the tracked actor's loss matters. Losing sight of something
            // else must not touch the target.
            if (Actor == GetTargetActor())
            {
                if (UBlackboardComponent* BB = GetBlackboardComponent())
                {
                    BB->SetValueAsVector(BBKeys::LastKnownLocation, Actor->GetActorLocation());
                }

                // During SoundOverload the target stays, so the enemy keeps chasing by sound.
                // HandleStatusRemoved clears it when the overload ends.
                if (!HasSoundOverload())
                {
                    SetTargetActor(nullptr);
                }
            }
        }
    }
    else if (Stimulus.Type == HearingID && Stimulus.WasSuccessfullySensed())
    {
        if (NoiseQueueComp)
        {
            NoiseQueueComp->PushNoise(Stimulus.StimulusLocation, Actor);
        }
    }
}

// --- Status handlers ---

void AEnemyAIController::HandleStatusAdded(FGameplayTag Tag, UObject* MyInstigator)
{
    if (Tag.MatchesTagExact(EnemyTags::Status_Stunned))
    {
        if (UBehaviorTreeComponent* BT = GetBTComponent())
        {
            BT->StopTree(EBTStopMode::Safe);
        }
        StopMovement();
    }
    else if (Tag.MatchesTagExact(EnemyTags::Status_Blinded))
    {
        // Save the last known location before dropping the target. Overload keeps tracking.
        if (!HasSoundOverload())
        {
            if (AActor* Current = GetTargetActor())
            {
                if (UBlackboardComponent* BB = GetBlackboardComponent())
                {
                    BB->SetValueAsVector(BBKeys::LastKnownLocation, Current->GetActorLocation());
                }
                SetTargetActor(nullptr);
            }
        }
        bCanSeeTarget = false;
        SetSightEnabled(false);
    }
    else if (Tag.MatchesTagExact(EnemyTags::Status_Rage))
    {
        RecalculateSightRange();
    }
    else if (Tag.MatchesTagExact(EnemyTags::Status_SoundOverload))
    {
        // A noise only takes over the target if the source is killable, and never
        // replaces a target we can currently see.
        AActor* NoiseSource = Cast<AActor>(MyInstigator);
        const bool bKeepVisibleTarget = bCanSeeTarget && GetTargetActor() != nullptr;
        if (NoiseSource && Cast<IKillable>(NoiseSource) && !bKeepVisibleTarget)
        {
            SetTargetActor(NoiseSource);
        }
    }
}

void AEnemyAIController::HandleStatusRemoved(FGameplayTag Tag, EStatusRemovalReason Reason)
{
    if (Tag.MatchesTagExact(EnemyTags::Status_Stunned))
    {
        // The GameMode owns the tree restart during a reset.
        if (Reason != EStatusRemovalReason::Reset && BehaviorTreeAsset)
        {
            if (UBehaviorTreeComponent* BT = GetBTComponent())
            {
                BT->StartTree(*BehaviorTreeAsset);
            }
        }
    }
    else if (Tag.MatchesTagExact(EnemyTags::Status_Blinded))
    {
        SetSightEnabled(true);
        if (UAIPerceptionComponent* Perc = GetPerceptionComponent())
        {
            Perc->RequestStimuliListenerUpdate();
        }
    }
    else if (Tag.MatchesTagExact(EnemyTags::Status_Rage))
    {
        RecalculateSightRange();
    }
    else if (Tag.MatchesTagExact(EnemyTags::Status_SoundOverload))
    {
        // Overload is over, so the noise queue's overload flag goes too.
        if (NoiseQueueComp)
        {
            NoiseQueueComp->ClearOverloadFlag();
        }
        if (!bCanSeeTarget)
        {
            SetTargetActor(nullptr);
        }
    }
}

// --- Noise handlers ---

void AEnemyAIController::HandleNoiseOverload(AActor* MyInstigator)
{
    AEnemyCharacter* Enemy = GetEnemyCharacter();
    const UEnemyGameSettings* Settings = UEnemyGameSettings::Get();
    if (!Enemy || !Settings) return;

    if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
    {
        Status->ApplyStatus(
            EnemyTags::Status_SoundOverload,
            Settings->NoiseOverloadSeconds,
            EStatusStackPolicy::Refresh,
            MyInstigator);
    }
}

void AEnemyAIController::HandleInvestigateLocationChanged(FVector Location, bool bValid)
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;

    if (bValid)
    {
        BB->SetValueAsVector(BBKeys::InvestigateLocation, Location);
    }
    else
    {
        BB->ClearValue(BBKeys::InvestigateLocation);
    }
}

// --- Zones ---

void AEnemyAIController::HandleZoneCompleted(FGameplayTag /*ZoneId*/)
{
    ApplyRageIfThresholdMet();
}

void AEnemyAIController::ApplyRageIfThresholdMet()
{
    UPuzzleZoneSubsystem* Zones = GetWorld() ? GetWorld()->GetSubsystem<UPuzzleZoneSubsystem>() : nullptr;
    if (!Zones) return;

    const UEnemyGameSettings* Settings = UEnemyGameSettings::Get();
    const int32 Total = Zones->GetTotalZoneCount();
    if (Total <= 0) return;

    const int32 Completed = Zones->GetCompletedCount();

    // Epsilon guards against float precision at integer boundaries
    // (e.g. 3 * 0.6667 = 2.0001, which would ceil to 3 without this).
    const int32 Threshold = FMath::CeilToInt(
        Total * Settings->RageThresholdRatio - 0.001f);

    if (Completed < Threshold) return;

    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
        {
            Status->ApplyStatus(EnemyTags::Status_Rage, 0.0f, EStatusStackPolicy::Ignore, Enemy);
        }
    }
}

// --- Reset ---

void AEnemyAIController::PrepareForReset()
{
    // 1. Stop the tree and movement.
    if (UBehaviorTreeComponent* BT = GetBTComponent())
    {
        BT->StopTree(EBTStopMode::Safe);
    }
    StopMovement();
    bCanSeeTarget = false;

    // 2. Blackboard. The state goes first so the speed recalculation in step 3 sees Patrol.
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->ClearValue(BBKeys::TargetActor);
        BB->ClearValue(BBKeys::LastKnownLocation);
        BB->ClearValue(BBKeys::InvestigateLocation);
        BB->ClearValue(BBKeys::CurrentPatrolPoint);
        // HomeLocation is kept. It is still where the enemy respawns.
    }
    SetAIState(EnemyTags::State_Patrol);

    // 3. Statuses. Reset reason, so Stunned does not restart the tree.
    AEnemyCharacter* Enemy = GetEnemyCharacter();
    if (Enemy)
    {
        if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
        {
            Status->ClearAll(EStatusRemovalReason::Reset);
        }
    }

    // 4. Noise and perception.
    if (NoiseQueueComp)
    {
        NoiseQueueComp->Clear();
    }
    if (UAIPerceptionComponent* Perc = GetPerceptionComponent())
    {
        Perc->ForgetAll();
    }

    // 5. Move the enemy home and reset the patrol route.
    if (Enemy)
    {
        Enemy->TeleportTo(Enemy->GetHomeLocation(), Enemy->GetActorRotation());
        if (UPatrolRouteComponent* Patrol = Enemy->GetPatrolComponent())
        {
            Patrol->ResetIndex();
        }
        Enemy->RecalculateMovementStats();
    }
}

void AEnemyAIController::ResumeAfterReset()
{
    if (UBehaviorTreeComponent* BT = GetBTComponent())
    {
        if (BehaviorTreeAsset)
        {
            BT->StartTree(*BehaviorTreeAsset);
        }
    }
}
