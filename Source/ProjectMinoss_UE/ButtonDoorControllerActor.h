// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DoorActor.h"
#include "ButtonActor.h"
#include "GameFramework/Actor.h"
#include "ButtonDoorControllerActor.generated.h"
class ADoorActor;
class AButtonActor;

UCLASS()
class PROJECTMINOSS_UE_API AButtonDoorControllerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButtonDoorControllerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Puzzle")
	TArray<TObjectPtr<AButtonActor>> Buttons;

	UPROPERTY(EditAnywhere, Category="Puzzle")
	TObjectPtr<ADoorActor> Door;

private:
	void CheckButtons();

	bool bDoorOpened = false;

};
