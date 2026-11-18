// Fill out your copyright notice in the Description page of Project Settings.


#include "LoanGameStateBase.h"

void ALoanGameStateBase::StartRun()
{
	bRunFinished = false;
}

void ALoanGameStateBase::FinishRun()
{
	if (bRunFinished)
	{
		return;
	}
	
	bRunFinished = true;
}
