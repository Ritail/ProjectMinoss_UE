// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LoanGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMINOSS_UE_API ALoanGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	void StartRun();
	void FinishRun();
	bool IsRunFinished() const { return bRunFinished; }
	
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	bool bRunFinished = false;
};
