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

void UInventoryActorComponent::AddKey(FGameplayTag KeyTag)
{
	// Tag vide ou clé déjà possédée : rien de nouveau, personne prévenir
	if (!KeyTag.IsValid() || HasKey(KeyTag))
	{

		return;

	}
	Keys.AddTag(KeyTag);
	OnKeyAdded.Broadcast(KeyTag);
}

void UInventoryActorComponent::UseKey(FGameplayTag KeyTag)
{
	if (Keys.HasTagExact(KeyTag))
	{
		Keys.RemoveTag(KeyTag);
		OnKeyUsed.Broadcast(KeyTag);
	}
}

bool UInventoryActorComponent::HasKey(FGameplayTag KeyTag) const
{
	return Keys.HasTagExact(KeyTag);
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


