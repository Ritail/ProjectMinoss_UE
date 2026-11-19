// NoiseQueueComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseQueue.h"
#include "NoiseQueueComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInvestigateLocationChanged, FVector /*Location*/, bool /*bValid*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNoiseOverload, AActor* /*Instigator*/);

UCLASS(ClassGroup=(Enemy), meta=(BlueprintSpawnableComponent))
class PROJECTMINOSS_UE_API UNoiseQueueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNoiseQueueComponent();

	/** Called by the perception listener in AEnemyAIController. */
	void PushNoise(const FVector& Location, AActor* Instigator);

	/** Empties the queue and invalidates the investigate point. Used by reset. */
	void Clear();

	/** Called when SoundOverload ends. Only clears the flag, not the queue. */
	void ClearOverloadFlag();

	int32 GetQueueSize() const { return Queue.Num(); }

	/** True from the moment the queue triggers until the overload ends. */
	bool IsOverloaded() const { return bOverloaded; }

	FOnInvestigateLocationChanged OnInvestigateLocationChanged;
	FOnNoiseOverload              OnNoiseOverload;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

private:
	void EvaluateTrigger(AActor* LatestInstigator);

	/** Broadcasts only when the investigate point actually changes. */
	void BroadcastInvestigate(bool bValid, const FVector& Location);

	FNoiseQueue Queue;
	bool bOverloaded = false;
	bool bLastInvestigateValid = false;
	FVector LastInvestigateLocation = FVector::ZeroVector;
};
