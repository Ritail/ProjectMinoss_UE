// Killable.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Killable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UKillable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTMINOSS_UE_API IKillable
{
	GENERATED_BODY()

	public:
	/** Called by the enemy when it successfully lands a kill on this actor. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Killable")
	void Kill(AActor* Instigator);
};