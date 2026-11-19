// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMINOSS_UE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TObjectPtr<ACharacter> Character;
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float CrouchSpeed = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldMove = false;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldCrouch = false;
};
