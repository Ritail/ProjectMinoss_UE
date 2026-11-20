// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterHandler.h"


// Sets default values
APlayerCharacterHandler::APlayerCharacterHandler()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCapsuleRadius(50.5f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(100.0f);
	GetCapsuleComponent()->InitCapsuleSize(50.0f, 100.0f);
	
	GetMesh()->SetSkeletalMeshAsset(SkeletalMesh);
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	// 1. Le Controller ne fait plus pivoter 
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	// 2. Player se tourne vers la direction de son déplacement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 3. La perche de caméra suit la souris
	SpringArm->bUsePawnControlRotation = true;
	// 4. La caméra se rapproche au lieu de traverser les murs
	SpringArm->bDoCollisionTest = true;
	
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
}

// Called when the game starts or when spawned
void APlayerCharacterHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacterHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UPrimitiveComponent* Held = PhysicsHandle->GetGrabbedComponent();
	if (Held && !IsValid(Held))
	{
		// La caisse a été détruite pendant le transport (étape 5) : on lâche
		PhysicsHandle->ReleaseComponent();
	}
	else if (Held)
	{
		// Le point cible suit Megan, dans la direction de la caméra
		PhysicsHandle->SetTargetLocation(GetActorLocation() + Camera->GetForwardVector() * HoldDistance);
	}
}

// Called to bind functionality to input
void APlayerCharacterHandler::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered,this,&APlayerCharacterHandler::PlayerMovement);
		Input->BindAction(RunAction, ETriggerEvent::Triggered,this,&APlayerCharacterHandler::PlayerRun);
		Input->BindAction(LookAction, ETriggerEvent::Triggered,this,&APlayerCharacterHandler::PlayerLook);
		Input->BindAction(CrouchAction, ETriggerEvent::Triggered,this,&APlayerCharacterHandler::PlayerCrouch);
		Input->BindAction(InteractAction, ETriggerEvent::Triggered,this,&APlayerCharacterHandler::Interacte);
		
		if (GrabAction)
		{
			// Started : un seul appel par clic
			Input->BindAction(GrabAction, ETriggerEvent::Started, this, &APlayerCharacterHandler::Grab);
		}
		else
		{

			UE_LOGFMT(LogTemp, Error, "{0} : GrabAction non assignée",

			GetName());
		}
		if (ThrowAction)
		{

			Input->BindAction(ThrowAction, ETriggerEvent::Started, this,&APlayerCharacterHandler::Throw);
		}
		else
		{

			UE_LOGFMT(LogTemp, Error, "{0} : ThrowAction non assignée",

			GetName());
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Warning, "Don't IMC");
	}
}

void APlayerCharacterHandler::PlayerMovement(const FInputActionValue& Value)
{
	const FVector2D MoveVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator YawRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
		
		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(Forward,MoveVector.X);
		AddMovementInput(Right,MoveVector.Y);
	}
}

void APlayerCharacterHandler::PlayerRun(const FInputActionValue& Value)
{
	const bool Run = Value.Get<bool>();

	if (Controller)
	{
		if (Run)
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 300.f;
		}
	}
}

void APlayerCharacterHandler::PlayerLook(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void APlayerCharacterHandler::PlayerCrouch(const FInputActionValue& Value)
{
	const bool crouch = Value.Get<bool>();
	
	if (Controller)
	{
		if (crouch)
		{
			Crouch(true);
		}
		else
		{
			UnCrouch(true);
		}
	}
}

AActor* APlayerCharacterHandler::FindInteractable() const
{
	FHitResult Hit;
	if (!TraceFromCamera(InteractRange, Hit))
	{
		return nullptr;
	}
	
	AActor* HitActor = Hit.GetActor();
	if (!HitActor || !HitActor->Implements<UInteractable>())
	{
		return nullptr;
	}
	
	if (FVector::Dist(GetActorLocation(), Hit.ImpactPoint) >InteractRange)
	{
		return nullptr;
	}
	
	return HitActor;
}

bool APlayerCharacterHandler::TraceFromCamera(float Range, FHitResult& OutHit) const
{
	if (!Camera || !SpringArm)
	{
		return false;
	}
	// Le rayon part de la caméra, derrière Megan : on ajoute la longueurdu bras

	const FVector Start = Camera->GetComponentLocation();
	const float TraceLength = SpringArm->TargetArmLength + Range;
	const FVector End = Start + Camera->GetForwardVector() * TraceLength;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start,
	End, ECC_Interaction, Params);

	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green :

	FColor::Red, false, 1.f);
	return bHit;
}

void APlayerCharacterHandler::Interacte()
{
	if (AActor* Target = FindInteractable())
	{
		IInteractable::Execute_Interact(Target, this);
	}
}

void APlayerCharacterHandler::Grab()
{
	// Le même bouton attrape et pose
	if (PhysicsHandle->GetGrabbedComponent())
	{
		Release();
		return;
	}
	
	FHitResult Hit;
	if (!TraceFromCamera(GrabRange, Hit))
	{
		return;
	}
	
	// On n'attrape que ce que le moteur physique déplace
	UPrimitiveComponent* Target = Hit.GetComponent();
	if (!Target || !Target->IsSimulatingPhysics())
	{
		return;
	}
	
	if (Target->GetMass() > MaxGrabMassKg)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f,

			FColor::Orange, TEXT("Trop lourd"));
		}
		return;
	}
	// Pendant le transport, la caisse ne pousse pas Megan
	Target->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	// ... et n'arrête pas le rayon de E : on peut ouvrir une porte en portant une caisse

	Target->SetCollisionResponseToChannel(ECC_Interaction, ECR_Ignore);
	PhysicsHandle->GrabComponentAtLocationWithRotation(Target, NAME_None,

	Hit.ImpactPoint, Target->GetComponentRotation());
}

void APlayerCharacterHandler::Release()
{
	UPrimitiveComponent* Held = PhysicsHandle->GetGrabbedComponent();
	if (!Held)
	{
		return;
	}
	Held->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Held->SetCollisionResponseToChannel(ECC_Interaction, ECR_Block);
	PhysicsHandle->ReleaseComponent();
}

void APlayerCharacterHandler::Throw()
{
	UPrimitiveComponent* Held = PhysicsHandle->GetGrabbedComponent();
	if (!Held)
	{

		return;

	}
	// D'abord lâcher : sinon le Physics Handle retient la caisse etannule l'impulsion
	Release();
	// bVelChange = true : ThrowSpeed est la vitesse donnée, quelle que soit la masse

	Held->AddImpulse(Camera->GetForwardVector() * ThrowSpeed, NAME_None,true);
}

FText APlayerCharacterHandler::GetInteractionText() const
{
	if (AActor* Target = FindInteractable())
	{
		return IInteractable::Execute_GetInteractionText(Target);
	}

	return FText::GetEmpty();
}

void APlayerCharacterHandler::DeathPlayer()
{
	
}

