// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "KeyDataAsset.generated.h"
class UStaticMesh;

/**
 * 
 */
UCLASS(BlueprintType)
class PROJECTMINOSS_UE_API UKeyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Key", meta = (Categories = "Item"))
	FGameplayTag Tag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Key")
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Key")
	FLinearColor Color = FLinearColor::White;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Key")
	TObjectPtr<UStaticMesh> Mesh;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Key")
	bool bIsConsumable;
};
