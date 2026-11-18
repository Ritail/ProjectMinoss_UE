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
    // Perceiver setup
    UAIPerceptionComponent* Perc = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    SetPerceptionComponent(*Perc);
    
    CreateDefaultSubobject<UNoiseQueueComponent>(TEXT("NoiseQueue"));

    UAISenseConfig_Sight* Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    Sight->DetectionByAffiliation.bDetectEnemies    = true;
    Sight->DetectionByAffiliation.bDetectNeutrals   = true;
    Sight->DetectionByAffiliation.bDetectFriendlies = false;
    Sight->LoseSightRadius           = 1800.f;
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

    // Baseline sight radius (settings may override in OnPossess)
    BaseSightRadius = Sight->SightRadius;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);
    if (!Enemy) return;

    // Apply settings to perception.
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

    // 2. Write BB defaults AFTER RunBehaviorTree (doc §Init order).
    BBComp->SetValueAsVector(BBKeys::HomeLocation, Enemy->GetHomeLocation());
    BBComp->ClearValue(BBKeys::TargetActor);
    BBComp->ClearValue(BBKeys::LastKnownLocation);
    BBComp->ClearValue(BBKeys::InvestigateLocation);
    BBComp->ClearValue(BBKeys::CurrentPatrolPoint);
    BBComp->SetValue<UBlackboardKeyType_GameplayTag>(BBKeys::StateTag, EnemyTags::State_Patrol);

    // 3. Bind status component.
    if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
    {
        Status->OnStatusAdded.AddUObject(this, &AEnemyAIController::HandleStatusAdded);
        Status->OnStatusRemoved.AddUObject(this, &AEnemyAIController::HandleStatusRemoved);
    }

    // 4. Bind noise component (owned by controller per doc §5.3).
    if (UNoiseQueueComponent* Noise = FindComponentByClass<UNoiseQueueComponent>())
    {
        Noise->OnNoiseOverload.AddUObject(this, &AEnemyAIController::HandleNoiseOverload);
        Noise->OnInvestigateLocationChanged.AddUObject(this, &AEnemyAIController::HandleInvestigateLocationChanged);
    }

    // 5. Bind zone subsystem.
    if (UPuzzleZoneSubsystem* Zones = GetWorld()->GetSubsystem<UPuzzleZoneSubsystem>())
    {
        Zones->OnZoneCompleted.AddDynamic(this, &AEnemyAIController::HandleZoneCompleted);
    }

    // 6. Bind perception.
    if (UAIPerceptionComponent* Perc = GetPerceptionComponent())
    {
        Perc->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::HandleTargetPerceptionUpdated);
    }

    // 7. Notify character.
    Enemy->OnAIPossessed(this);
}

void AEnemyAIController::EndPlay(const EEndPlayReason::Type Reason)
{
    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
        {
            Status->OnStatusAdded.RemoveAll(this);
            Status->OnStatusRemoved.RemoveAll(this);
        }
        if (UNoiseQueueComponent* Noise = FindComponentByClass<UNoiseQueueComponent>())
        {
            Noise->OnNoiseOverload.RemoveAll(this);
            Noise->OnInvestigateLocationChanged.RemoveAll(this);
        }
    }

    if (UPuzzleZoneSubsystem* Zones = GetWorld() ? GetWorld()->GetSubsystem<UPuzzleZoneSubsystem>() : nullptr)
    {
        Zones->OnZoneCompleted.RemoveAll(this);
    }

    if (UAIPerceptionComponent* Perc = GetPerceptionComponent())
    {
        Perc->OnTargetPerceptionUpdated.RemoveAll(this);
    }

    Super::EndPlay(Reason);
}

AEnemyCharacter* AEnemyAIController::GetEnemyCharacter() const
{
    return Cast<AEnemyCharacter>(GetPawn());
}

UBehaviorTreeComponent* AEnemyAIController::GetBTComponent() const
{
    return Cast<UBehaviorTreeComponent>(BrainComponent);
}

void AEnemyAIController::ConfigurePerception()
{
    const UEnemyGameSettings* Settings = UEnemyGameSettings::Get();
    UAIPerceptionComponent* Perc = GetPerceptionComponent();
    if (!Settings || !Perc) return;

    if (UAISenseConfig_Sight* Sight = Cast<UAISenseConfig_Sight>(
        Perc->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>())))
    {
        Sight->SightRadius                = Settings->EnemySightRadius;
        Sight->LoseSightRadius            = Settings->EnemySightRadius * 1.2f;
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
            if (Status->HasStatus(EnemyTags::Status_Rage))
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
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;

    const FAISenseID SightID   = UAISense::GetSenseID<UAISense_Sight>();
    const FAISenseID HearingID = UAISense::GetSenseID<UAISense_Hearing>();

    if (Stimulus.Type == SightID)
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            bCanSeeTarget = true;
            BB->SetValueAsObject(BBKeys::TargetActor, Actor);
        }
        else
        {
            bCanSeeTarget = false;
            // Write LKP before clearing TargetActor so Search has a destination.
            if (AActor* Current = Cast<AActor>(BB->GetValueAsObject(BBKeys::TargetActor)))
            {
                BB->SetValueAsVector(BBKeys::LastKnownLocation, Current->GetActorLocation());
            }
            BB->ClearValue(BBKeys::TargetActor);
        }
    }
    else if (Stimulus.Type == HearingID && Stimulus.WasSuccessfullySensed())
    {
        if (UNoiseQueueComponent* Noise = FindComponentByClass<UNoiseQueueComponent>())
        {
            Noise->PushNoise(Stimulus.StimulusLocation, Actor);
        }
    }
}

// --- Status handlers ---

void AEnemyAIController::HandleStatusAdded(FGameplayTag Tag, UObject* MyInstigator)
{
    UBlackboardComponent* BB = GetBlackboardComponent();

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
        // Doc §4.3: copy LKP before clearing TargetActor.
        // BUT preserve TargetActor if Overload is keeping Chase alive — overload's
        // job is "chase without sight", and blind without overload needs to fall to Search.
        const bool bOverloadActive =
            GetEnemyCharacter() && GetEnemyCharacter()->GetStatusComponent() &&
            GetEnemyCharacter()->GetStatusComponent()->HasStatus(EnemyTags::Status_SoundOverload);

        if (!bOverloadActive && BB)
        {
            if (AActor* Current = Cast<AActor>(BB->GetValueAsObject(BBKeys::TargetActor)))
            {
                BB->SetValueAsVector(BBKeys::LastKnownLocation, Current->GetActorLocation());
                BB->ClearValue(BBKeys::TargetActor);
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
        if (BB && MyInstigator)
        {
            if (AActor* Inst = Cast<AActor>(MyInstigator))
            {
                BB->SetValueAsObject(BBKeys::TargetActor, Inst);
            }
        }
    }
}

void AEnemyAIController::HandleStatusRemoved(FGameplayTag Tag, EStatusRemovalReason Reason)
{
    UBlackboardComponent* BB = GetBlackboardComponent();

    if (Tag.MatchesTagExact(EnemyTags::Status_Stunned))
    {
        // GameMode owns the tree restart on reset — don't race it.
        if (Reason != EStatusRemovalReason::Reset)
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
        if (!bCanSeeTarget && BB)
        {
            BB->ClearValue(BBKeys::TargetActor);
        }
    }
}

// --- Noise handlers ---

void AEnemyAIController::HandleNoiseOverload(AActor* MyInstigator)
{
    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
        {
            const UEnemyGameSettings* Settings = UEnemyGameSettings::Get();
            Status->ApplyStatus(
                EnemyTags::Status_SoundOverload,
                Settings->NoiseOverloadSeconds,
                EStatusStackPolicy::Refresh,
                MyInstigator);
        }
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
    const int32 Threshold = FMath::CeilToInt(Total * Settings->RageThresholdRatio);

    if (Completed < Threshold) return;

    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
        {
            // Infinite duration, Ignore stacking (idempotent).
            Status->ApplyStatus(EnemyTags::Status_Rage, 0.0f, EStatusStackPolicy::Ignore, Enemy);
        }
    }
}

void AEnemyAIController::PrepareForReset()
{
    // 1. Stop BT + movement
    if (UBehaviorTreeComponent* BT = GetBTComponent())
    {
        BT->StopTree(EBTStopMode::Safe);
    }
    StopMovement();
    bCanSeeTarget = false;

    // 2-4. Clear component state
    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        if (UStatusEffectComponent* Status = Enemy->GetStatusComponent())
        {
            Status->ClearAll(EStatusRemovalReason::Reset);
        }
    }
    if (UNoiseQueueComponent* Noise = FindComponentByClass<UNoiseQueueComponent>())
    {
        Noise->Clear();
    }
    if (UAIPerceptionComponent* Perc = GetPerceptionComponent())
    {
        Perc->ForgetAll();
    }

    // 5. Explicit per-key BB clear (RestartTree/StartTree do NOT reset BB memory)
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->ClearValue(BBKeys::TargetActor);
        BB->ClearValue(BBKeys::LastKnownLocation);
        BB->ClearValue(BBKeys::InvestigateLocation);
        BB->ClearValue(BBKeys::CurrentPatrolPoint);
        BB->SetValue<UBlackboardKeyType_GameplayTag>(BBKeys::StateTag, EnemyTags::State_Patrol);
        // HomeLocation is preserved — it's still where the enemy respawns.
    }

    // 6. Teleport enemy home + reset patrol index
    if (AEnemyCharacter* Enemy = GetEnemyCharacter())
    {
        Enemy->SetActorLocation(Enemy->GetHomeLocation());
        if (UPatrolRouteComponent* Patrol = Enemy->GetPatrolComponent())
        {
            Patrol->ResetIndex();
        }
    }
}

void AEnemyAIController::ResumeAfterReset()
{
    // 9. Clean tree start
    if (UBehaviorTreeComponent* BT = GetBTComponent())
    {
        if (BehaviorTreeAsset)
        {
            BT->StartTree(*BehaviorTreeAsset);
        }
    }
}