// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonDoorControllerActor.h"

// Sets default values
AButtonDoorControllerActor::AButtonDoorControllerActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AButtonDoorControllerActor::BeginPlay()
{
	Super::BeginPlay();
	
	for (AButtonActor* Button : Buttons)
	{
		if (Button)
		{
			Button->OnStateChanged.AddUObject(this, &AButtonDoorControllerActor::CheckButtons);
		}
	}
}

void AButtonDoorControllerActor::CheckButtons()
{
	if (!Door || Buttons.Num() == 0)
	{
		return;
	}

	bool bAllPressed = true;
	for (const AButtonActor* Button : Buttons)
	{
		if (!Button || !Button->IsPressed())
		{
			bAllPressed = false;
			break;
		}
	}
	
	if (bAllPressed != bDoorOpened)
	{
		bDoorOpened = bAllPressed;
		if (bDoorOpened)
		{
			Door->Open();
		}
	}
}




