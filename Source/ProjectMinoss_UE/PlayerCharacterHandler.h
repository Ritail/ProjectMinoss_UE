// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Interactable.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "ProjectMinossCollision.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterHandler.generated.h"

UCLASS()
class PROJECTMINOSS_UE_API APlayerCharacterHandler : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacterHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "Grab")
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMesh> SkeletalMesh;
	//Camera
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;
	
	//Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TObjectPtr<UInputAction> RunAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TObjectPtr<UInputAction> CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TObjectPtr<UInputAction> InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> GrabAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ThrowAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta =
	(AllowPrivateAccess = "true"))
	float InteractRange = 250.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grab", meta =
(AllowPrivateAccess = "true", ClampMin = "0", Units = "cm"))
	float GrabRange = 250.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grab", meta =
	(AllowPrivateAccess = "true", ClampMin = "0", Units = "cm"))
	float HoldDistance = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grab", meta =
	(AllowPrivateAccess = "true", ClampMin = "0", Units = "kg"))
	float MaxGrabMassKg = 50.f;
	// Vitesse donnée à la caisse lancée, quelle que soit sa masse
	UPROPERTY(EditDefaultsOnly, Category = "Grab", meta =

	(AllowPrivateAccess = "true", ClampMin = "0"))
	float ThrowSpeed = 1500.f;
	
	// Fonctions d'input : pas de UFUNCTION (TP 1 ter, 2.2)
	void Grab();
	void Release();
	void Throw();
	
	UFUNCTION()
	void PlayerMovement(const FInputActionValue& Value);
	
	UFUNCTION()
	void PlayerRun(const FInputActionValue& Value);
	
	UFUNCTION()
	void PlayerLook(const FInputActionValue& Value);
	
	UFUNCTION()
	void PlayerCrouch(const FInputActionValue& Value);
	
	void Interacte();
	
	
	
	// Renvoie l'acteur interactable visé, ou nullptr
	AActor* FindInteractable() const;
	
	UFUNCTION(BlueprintCallable)
	FText GetInteractionText() const;
	
	// Renvoie true si le rayon a touché quelque chose (résultat dans OutHit).
	bool TraceFromCamera(float Range, FHitResult& OutHit) const;
	
	void DeathPlayer();
	
};
