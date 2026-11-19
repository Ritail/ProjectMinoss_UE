// TestCharacter.cpp
#include "TestCharacter.h"
#include "HealthComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

ATestCharacter::ATestCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // Default capsule (matches the engine ACharacter template).
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

    // Third-person controller setup: character faces movement direction.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw   = false;
    bUseControllerRotationRoll  = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    GetCharacterMovement()->JumpZVelocity = 500.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 400.f;

    // Camera
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Components the enemy AI expects.
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

    NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));
}

void ATestCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HealthComponent)
    {
        HealthComponent->OnDied.AddDynamic(this, &ATestCharacter::HandleDied);
        HealthComponent->OnRevived.AddDynamic(this, &ATestCharacter::HandleRevived);
    }

    // Start the footstep timer. It gates on velocity inside EmitFootstepNoise(),
    // so leaving it running continuously is safe and cheap.
    GetWorldTimerManager().SetTimer(
        FootstepTimer, this, &ATestCharacter::EmitFootstepNoise,
        FootstepIntervalSeconds, /*bLoop=*/true);
}

void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Add the input mapping context.
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATestCharacter::HandleMove);
        }
        if (LookAction)
        {
            EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATestCharacter::HandleLook);
        }
        if (JumpAction)
        {
            EIC->BindAction(JumpAction, ETriggerEvent::Started,   this, &ACharacter::Jump);
            EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        }
    }
}

void ATestCharacter::HandleMove(const FInputActionValue& Value)
{
    const FVector2D Input = Value.Get<FVector2D>();
    if (!Controller || Input.IsNearlyZero()) return;

    // Project input onto the controller's yaw plane so WASD feels camera-relative.
    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const FVector Right   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    AddMovementInput(Forward, Input.Y);
    AddMovementInput(Right,   Input.X);
}

void ATestCharacter::HandleLook(const FInputActionValue& Value)
{
    const FVector2D Input = Value.Get<FVector2D>();
    AddControllerYawInput(Input.X);
    AddControllerPitchInput(Input.Y);
}

void ATestCharacter::EmitFootstepNoise()
{
    if (!NoiseEmitter) return;
    if (HealthComponent && HealthComponent->IsDead()) return;

    // Only emit when actually moving horizontally (>= 10 uu/s).
    if (GetVelocity().SizeSquared2D() < FMath::Square(10.f)) return;

    NoiseEmitter->MakeNoise(this, FootstepLoudness, GetActorLocation());
}

void ATestCharacter::Kill_Implementation(AActor* MyInstigator)
{
    if (!HealthComponent || HealthComponent->IsDead()) return;
    HealthComponent->Die(MyInstigator);
}

void ATestCharacter::HandleDied(AActor* /*Killer*/)
{
    // Stop the timer immediately so a dead player doesn't keep feeding the noise queue.
    GetWorldTimerManager().ClearTimer(FootstepTimer);
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "You Died");
}

void ATestCharacter::HandleRevived()
{
    // Restart the footstep timer. Called when GameMode reset restores the player.
    GetWorldTimerManager().SetTimer(
        FootstepTimer, this, &ATestCharacter::EmitFootstepNoise,
        FootstepIntervalSeconds, /*bLoop=*/true);
}