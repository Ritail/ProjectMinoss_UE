// TestCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Killable.h"
#include "TestCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UHealthComponent;
class UPawnNoiseEmitterComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

/**
 * Minimal playable character for testing the enemy AI.
 *  - Implements IKillable so BTTask_TryKillPlayer has something to act on.
 *  - Owns a UHealthComponent whose OnDied the GameMode binds to.
 *  - Implements IGenericTeamAgentInterface (team = Player).
 *  - Emits footstep noise via UPawnNoiseEmitterComponent so the enemy's
 *    hearing sense can drive the noise-overload mechanic (doc §5).
 *  - Third-person camera + Enhanced Input. No mesh required — works "god view".
 */
UCLASS()
class PROJECTMINOSS_UE_API ATestCharacter : public ACharacter,
                                            public IGenericTeamAgentInterface,
                                            public IKillable
{
    GENERATED_BODY()

public:
    ATestCharacter();

    static constexpr uint8 PlayerTeamId = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UHealthComponent> HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UPawnNoiseEmitterComponent> NoiseEmitter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    // --- IGenericTeamAgentInterface ---
    virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(PlayerTeamId); }
    virtual void SetGenericTeamId(const FGenericTeamId& /*InTeamID*/) override { /* fixed team */ }

    // --- IKillable ---
    virtual void Kill_Implementation(AActor* MyInstigator) override;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // Input assets — assign in a BP_TestCharacter subclass.
    UPROPERTY(EditDefaultsOnly, Category="Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    TObjectPtr<UInputAction> JumpAction;

    // Noise tuning (doc §5.4 defaults).
    UPROPERTY(EditDefaultsOnly, Category="AI|Noise", meta=(ClampMin="0.05"))
    float FootstepIntervalSeconds = 0.4f;

    UPROPERTY(EditDefaultsOnly, Category="AI|Noise", meta=(ClampMin="0.0"))
    float FootstepLoudness = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category="AI|Noise", meta=(ClampMin="1.0"))
    float FootstepMaxRange = 2500.f;

private:
    void HandleMove(const FInputActionValue& Value);
    void HandleLook(const FInputActionValue& Value);
    void EmitFootstepNoise();

    UFUNCTION()
    void HandleDied(AActor* Killer);

    UFUNCTION()
    void HandleRevived();

    FTimerHandle FootstepTimer;
};