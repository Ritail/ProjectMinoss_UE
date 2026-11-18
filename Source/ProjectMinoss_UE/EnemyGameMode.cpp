// EnemyGameMode.cpp
#include "EnemyGameMode.h"
#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "PuzzleZoneSubsystem.h"
#include "HealthComponent.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

AEnemyGameMode::AEnemyGameMode()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEnemyGameMode::StartPlay()
{
    Super::StartPlay();

    // Cache player start pose + bind death.
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            CachedPlayerStartLocation = Pawn->GetActorLocation();
            CachedPlayerStartRotation = Pawn->GetActorRotation();

            if (UHealthComponent* Health = Pawn->FindComponentByClass<UHealthComponent>())
            {
                Health->OnDied.AddUniqueDynamic(this, &AEnemyGameMode::HandlePlayerDied);
            }
            else
            {
                UE_LOG(LogTemp, Warning,
                    TEXT("AEnemyGameMode: player pawn '%s' has no UHealthComponent — "
                         "player death will not trigger reset. Add UHealthComponent to the player BP "
                         "and implement IKillable::Kill to call Health->Die()."),
                    *Pawn->GetName());
            }
        }
    }
}

void AEnemyGameMode::HandlePlayerDied(AActor* /*Killer*/)
{
    ResetGame();
}

void AEnemyGameMode::ResetGame()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Find the single enemy controller (the game has exactly one, per design).
    AEnemyAIController* EnemyAI = nullptr;
    for (TActorIterator<AEnemyAIController> It(World); It; ++It)
    {
        EnemyAI = *It;
        break;
    }

    // Steps 1-6 — enemy stop, clear, clear noise, forget perception, clear BB, teleport.
    if (EnemyAI)
    {
        EnemyAI->PrepareForReset();
    }

    // Step 7 — player teleport + inventory/world items.
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (PlayerPawn)
    {
        PlayerPawn->SetActorLocation(CachedPlayerStartLocation, /*bSweep=*/false, nullptr,
                                     ETeleportType::TeleportPhysics);
        PlayerPawn->SetActorRotation(CachedPlayerStartRotation, ETeleportType::TeleportPhysics);
        ResetPlayerAndWorld(PlayerPawn);
    }

    // Step 8 — subsystem reset (fires OnZonesReset internally).
    if (UPuzzleZoneSubsystem* Zones = World->GetSubsystem<UPuzzleZoneSubsystem>())
    {
        Zones->Reset();
    }

    // Step 9 — clean BT start (after zones so patrol sees fresh completion state).
    if (EnemyAI)
    {
        EnemyAI->ResumeAfterReset();
    }

    // Step 10 — broadcast LAST.
    OnGameReset.Broadcast();
}

void AEnemyGameMode::ResetPlayerAndWorld_Implementation(APawn* /*PlayerPawn*/)
{
    // Override in a BP subclass for inventory / world-item reset.
    // Stub here so the call site in ResetGame stays clean.
}