// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "Components/ActorComponent.h"
#include "InventoryActorComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyAdded, FGameplayTag,
KeyTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyUsed, FGameplayTag,
KeyTag);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTMINOSS_UE_API UInventoryActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnKeyAdded OnKeyAdded;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnKeyAdded OnKeyUsed;
	
	// Sets default values for this component's properties
	UInventoryActorComponent();

	static UInventoryActorComponent* FindInventory(const AActor* Actor);
	
	void AddKey(FGameplayTag KeyTag);
	void UseKey(FGameplayTag KeyTag);
	bool HasKey(FGameplayTag KeyTag) const;
	
protected:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category ="Inventory")
	FGameplayTagContainer Keys;

		
};
