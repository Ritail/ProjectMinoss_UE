// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interactable.h"
#include "Components/TextRenderComponent.h"
#include "KeyDataAsset.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "DoorActor.generated.h"
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECTMINOSS_UE_API ADoorActor : public AActor,public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorActor();
	void Open();
	void Close();
	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual FText GetInteractionText_Implementation() const override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Composants visible mais non supprimable
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<USceneComponent> Hinge;
	
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<UBoxComponent> Trigger;
	
	// Réglages : une valeur par défaut dans BP_Door, mais chaque porte
	// du niveau peut être différente. Un des rares cas d'EditAnywhere.
	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenAngle = 90.f;
	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenSpeed = 3.f;
	UPROPERTY(EditAnywhere, Category = "Door")
	bool bOpensWithKey = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly,SaveGame ,Category = "State")
	bool bIsOpen = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,SaveGame ,Category = "State")
	bool bIsPressurePlate = false;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,SaveGame ,Category = "Key")
	bool bIsUsedKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key",
	meta = (EditCondition = "bIsUsedKey", EditConditionHides))
	TObjectPtr<UKeyDataAsset> KeyData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key",
	meta = (EditCondition = "bIsUsedKey", EditConditionHides))
	FGameplayTag RequiredKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TextDoor")
	UTextRenderComponent* InteractionText;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnInteractionTriggerBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	
);

	UFUNCTION()
	void OnInteractionTriggerEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	float CurrentYaw = 0.f;

};
