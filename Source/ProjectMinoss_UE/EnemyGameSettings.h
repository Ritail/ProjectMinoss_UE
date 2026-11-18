// EnemyGameSettings.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "EnemyGameSettings.generated.h"

UCLASS(config=Game, defaultconfig, meta=(DisplayName="Enemy Game Settings"))
class PROJECTMINOSS_UE_API UEnemyGameSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UEnemyGameSettings();

    virtual FName GetCategoryName() const override { return TEXT("Game"); }

    static const UEnemyGameSettings* Get() { return GetDefault<UEnemyGameSettings>(); }

    // --- Zone configuration (source of truth for total count) ---
    UPROPERTY(config, EditAnywhere, Category="Zones", meta=(Categories="Zone"))
    TArray<FGameplayTag> ZoneIds;

    // --- Rage ---
    UPROPERTY(config, EditAnywhere, Category="Rage", meta=(ClampMin="0.0", ClampMax="1.0"))
    float RageThresholdRatio = 0.6667f;

    UPROPERTY(config, EditAnywhere, Category="Rage", meta=(ClampMin="1.0"))
    float RageSpeedMultiplier = 1.4f;

    UPROPERTY(config, EditAnywhere, Category="Rage", meta=(ClampMin="1.0"))
    float RageSightMultiplier = 1.3f;

    // --- Noise ---
    UPROPERTY(config, EditAnywhere, Category="Noise", meta=(ClampMin="1"))
    int32 NoiseThreshold = 3;

    UPROPERTY(config, EditAnywhere, Category="Noise", meta=(ClampMin="0.1"))
    float NoiseWindowSeconds = 4.f;

    UPROPERTY(config, EditAnywhere, Category="Noise", meta=(ClampMin="0.1"))
    float NoiseOverloadSeconds = 6.f;

    // --- Baseline enemy stats ---
    UPROPERTY(config, EditAnywhere, Category="Stats", meta=(ClampMin="0.0"))
    float EnemyMoveSpeedWalk = 200.f;

    UPROPERTY(config, EditAnywhere, Category="Stats", meta=(ClampMin="0.0"))
    float EnemyMoveSpeedChase = 500.f;

    UPROPERTY(config, EditAnywhere, Category="Stats", meta=(ClampMin="0.0"))
    float EnemySightRadius = 1500.f;

    UPROPERTY(config, EditAnywhere, Category="Stats", meta=(ClampMin="0.0"))
    float EnemySightFOVDegrees = 70.f;

    UPROPERTY(config, EditAnywhere, Category="Stats", meta=(ClampMin="0.0"))
    float EnemyHearingRange = 2500.f;
};