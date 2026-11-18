// Fill out your copyright notice in the Description page of Project Settings.


#include "LoanGameModeBase.h"

#include "LoanGameStateBase.h"

void ALoanGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (ALoanGameStateBase* GameStateBase = GetGameState<ALoanGameStateBase>())
	{
		GameStateBase->StartRun();
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "{0} : Game State Class n'est pas LoanGameState", GetName());
	}
}
