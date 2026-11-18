// PatrolPoint.cpp
#include "PatrolPoint.h"
#include "Components/SceneComponent.h"

APatrolPoint::APatrolPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}