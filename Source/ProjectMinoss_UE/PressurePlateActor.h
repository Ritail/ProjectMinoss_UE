// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DoorActor.h"
#include "PressurePlateActor.generated.h"
class UBoxComponent;
class UStaticMeshComponent;
class ADoorActor;

UCLASS()
class PROJECTMINOSS_UE_API APressurePlateActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APressurePlateActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category = "Plate")
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere, Category = "Plate")
	TObjectPtr<UStaticMeshComponent> PlateMesh;
	UPROPERTY(VisibleAnywhere, Category = "Plate")
	TObjectPtr<UBoxComponent> Trigger;
	/** Masse minimale posée pour enfoncer la plaque. Megan seule ne
	
	compte pas. */

	UPROPERTY(EditAnywhere, Category = "Plate", meta = (ClampMin = "0",

	Units = "kg"))

	float RequiredMassKg = 30.f;
	// La porte commandée, choisie plaque par plaque (TP 1 ter, 2.1). Vide : aucune porte.

	UPROPERTY(EditInstanceOnly, Category = "Plate")
	TObjectPtr<ADoorActor> TargetDoor;
	UPROPERTY(VisibleInstanceOnly, Category = "State")
	bool bIsPressed = false;
	UPROPERTY(VisibleInstanceOnly, Category = "State")
	float CurrentMassKg = 0.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// Recalcule la masse et ouvre ou ferme la porte si l'état change
	void UpdatePressure();
	float ComputeMassOnPlate() const;

};
