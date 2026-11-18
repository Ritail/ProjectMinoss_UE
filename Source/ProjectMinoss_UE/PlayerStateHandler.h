// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryActorComponent.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateHandler.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMINOSS_UE_API APlayerStateHandler : public APlayerState
{
	GENERATED_BODY()
	
	APlayerStateHandler();
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UInventoryActorComponent> Inventory;
};
