// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Interactable.h"
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta =
	(AllowPrivateAccess = "true"))
	float InteractRange = 250.f;
	
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
