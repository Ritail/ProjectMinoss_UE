// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerHandler.h"


APlayerControllerHandler::APlayerControllerHandler()
{
	bShouldPerformFullTickWhenPaused = true;
}

void APlayerControllerHandler::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
	{

		return;
	}
	
	if (!DefaultMappingContext)
	{

		UE_LOGFMT(LogTemp, Error, "{0} : DefaultMappingContext non assigné", GetName());
		return;
	}
	
	// Plus besoin de remonter Pawn -> Controller : on EST le controller
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
	ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>
	(GetLocalPlayer()))
	{

		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void APlayerControllerHandler::PauseSetup()
{
	if (IsPaused())
	{
		GEngine->RemoveOnScreenDebugMessage(1);
		SetPause(false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			30.0f,
			FColor::Green,
			TEXT("Jeu en pause - Appuyez sur P pour reprendre")
		);

		SetPause(true);
	}
}

void APlayerControllerHandler::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Input->BindAction(PauseAction,ETriggerEvent::Triggered,this,&APlayerControllerHandler::PauseSetup);
	}
}
