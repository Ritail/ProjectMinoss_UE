// EnemyCharacter.cpp
#include "EnemyCharacter.h"
#include "StatusEffectComponent.h"
#include "PatrolRouteComponent.h"
#include "EnemyGameplayTags.h"
#include "EnemyGameSettings.h"
#include "AIBlackboardKeys.h"
#include "AIController.h"
#include "BlackboardKeyType_GameplayTag.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    StatusComponent = CreateDefaultSubobject<UStatusEffectComponent>(TEXT("StatusComponent"));
    PatrolComponent = CreateDefaultSubobject<UPatrolRouteComponent>(TEXT("PatrolComponent"));
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    HomeLocation = GetActorLocation();

    if (StatusComponent)
    {
        StatusComponent->OnStatusAdded.AddUObject(this, &AEnemyCharacter::HandleStatusAdded);
        StatusComponent->OnStatusRemoved.AddUObject(this, &AEnemyCharacter::HandleStatusRemoved);
    }
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
    if (StatusComponent)
    {
        StatusComponent->OnStatusAdded.RemoveAll(this);
        StatusComponent->OnStatusRemoved.RemoveAll(this);
    }
    Super::EndPlay(Reason);
}

void AEnemyCharacter::OnAIPossessed(AAIController* AIC)
{
    RecalculateMovementStats();
}

void AEnemyCharacter::RecalculateMovementStats()
{
    const UEnemyGameSettings* Settings = UEnemyGameSettings::Get();
    if (!Settings) return;

    // Determine current state from BB.
    bool bChasing = false;
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
        {
            const FGameplayTag State =
                BB->GetValue<UBlackboardKeyType_GameplayTag>(BBKeys::StateTag);
            bChasing = State.MatchesTagExact(EnemyTags::State_Chase);
        }
    }

    float Speed = bChasing ? Settings->EnemyMoveSpeedChase : Settings->EnemyMoveSpeedWalk;

    // Compose multipliers from base — never multiply the current value.
    if (StatusComponent && StatusComponent->HasStatus(EnemyTags::Status_Rage))
    {
        Speed *= Settings->RageSpeedMultiplier;
    }

    GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AEnemyCharacter::HandleStatusAdded(FGameplayTag /*Tag*/, UObject* /*Instigator*/)
{
    RecalculateMovementStats();
}

void AEnemyCharacter::HandleStatusRemoved(FGameplayTag /*Tag*/, EStatusRemovalReason /*Reason*/)
{
    RecalculateMovementStats();
}