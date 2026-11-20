// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "KeyDataAsset.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "KeyHandler.generated.h"
class UBoxComponent;
class UStaticMeshComponent;
class UKeyDataAsset;

UCLASS()
class PROJECTMINOSS_UE_API AKeyHandler : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKeyHandler();
	virtual void Interact_Implementation(AActor* InteractingActor) override;
	// Appelée dans l'éditeur dès qu'on place ou modifie l'acteur
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "Key")
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere, Category = "Key")
	TObjectPtr<USceneComponent> Hinge;
	
	UPROPERTY(VisibleAnywhere, Category = "Key")
	TObjectPtr<UStaticMeshComponent> KeyMesh;
	UPROPERTY(VisibleAnywhere, Category = "Key")
	TObjectPtr<UBoxComponent> Trigger;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key")
	TObjectPtr<UKeyDataAsset> KeyData;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
