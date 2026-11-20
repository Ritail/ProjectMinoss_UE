// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerHandler.h"

#include "InventoryActorComponent.h"


APlayerControllerHandler::APlayerControllerHandler()
{
	bShouldPerformFullTickWhenPaused = true;
}

void APlayerControllerHandler::BeginPlay()
{
	Super::BeginPlay();
	
	if (UInventoryActorComponent* Inventory = UInventoryActorComponent::FindInventory(PlayerState))
	{
		Inventory->OnKeyAdded.AddDynamic(this,&APlayerControllerHandler::HandleKeyAdded);
		Inventory->OnKeyUsed.AddDynamic(this,&APlayerControllerHandler::HandleKeyUsed);
	}
	else
	{

		UE_LOGFMT(LogTemp, Warning, "{0} : pas d'inventaire sur le PlayerState", GetName());
	}
	
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

void APlayerControllerHandler::HandleKeyAdded(FGameplayTag KeyTag)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
			FString::Printf(TEXT("Clé %s ramassée"), *KeyTag.ToString()));
	}
}

void APlayerControllerHandler::HandleKeyUsed(FGameplayTag KeyTag)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
			FString::Printf(TEXT("Clé %s utilisée !"), *KeyTag.ToString()));
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
