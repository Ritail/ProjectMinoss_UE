// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ButtonActor.generated.h"
class UBoxComponent;
class UStaticMeshComponent;

DECLARE_MULTICAST_DELEGATE(FOnButtonStateChanged);

UCLASS()
class PROJECTMINOSS_UE_API AButtonActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButtonActor();
	virtual void Interact_Implementation(AActor* InteractingActor) override;
	FOnButtonStateChanged OnStateChanged;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPressed() const { return bIsPressed; }
	
	void SetPressed(bool bNewPressed);
	
	UPROPERTY(VisibleAnywhere, Category = "Button")
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere, Category = "Button")
	TObjectPtr<UStaticMeshComponent> BaseMesh;
	UPROPERTY(VisibleAnywhere, Category = "Button")
	TObjectPtr<UStaticMeshComponent> ButtonMesh;
	UPROPERTY(VisibleAnywhere, Category = "Button")
	TObjectPtr<UBoxComponent> Trigger;
	
	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bIsPressed = false;
	UPROPERTY(EditAnywhere, Category = "State")
	float PressedTranslate = 5.f;
	UPROPERTY(EditAnywhere, Category = "State")
	float PressedSpeed = 2.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private : 
	float CurrentTranslate = 0.f;
	FVector InitialRelativeLocation;
};
