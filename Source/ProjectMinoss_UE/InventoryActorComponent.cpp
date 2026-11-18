// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryActorComponent.h"

#include "GameFramework/PlayerState.h"

// Sets default values for this component's properties
UInventoryActorComponent::UInventoryActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

UInventoryActorComponent* UInventoryActorComponent::FindInventory(const AActor* Actor)
{
	
	if (!Actor)
	{

		return nullptr;
	}
	
	// 1. Sur l'acteur lui-même (un coffre, un garde, ou Megan)
	if (UInventoryActorComponent* Inventory = Actor->FindComponentByClass<UInventoryActorComponent>())
	{

		return Inventory;
	}
	
	// 2. Sur le PlayerState d'un pawn (pour survivre à la réapparition)
	if (const APawn* Pawn = Cast<APawn>(Actor))
	{

		if (const APlayerState* PlayerStateHandler = Pawn->GetPlayerState())
		{

			return PlayerStateHandler->FindComponentByClass<UInventoryActorComponent>();
		}
	}
	
	return nullptr;
}


