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

	void Clear();

	int32 GetQueueSize() const { return Queue.Num(); }

	/** True while the queue is above threshold — controller reads this on ApplyStatus paths. */
	bool IsOverloaded() const { return bOverloaded; }

	FOnInvestigateLocationChanged OnInvestigateLocationChanged;
	FOnNoiseOverload              OnNoiseOverload;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

private:
	void EvaluateTrigger(AActor* LatestInstigator);

	FNoiseQueue Queue;
	bool bOverloaded = false;
};

