// HealthComponent.cpp
#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::Die(AActor* Killer)
{
	if (bDead) return;
	bDead = true;
	OnDied.Broadcast(Killer);
}

void UHealthComponent::Revive()
{
	if (!bDead) return;
	bDead = false;
	OnRevived.Broadcast();
}