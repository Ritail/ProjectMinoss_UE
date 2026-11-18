// BTS_UpdateLastKnownLocation.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_UpdateLastKnownLocation.generated.h"

UCLASS(meta=(DisplayName="Update Last Known Location"))
class PROJECTMINOSS_UE_API UBTService_UpdateLastKnownLocation : public UBTService
{
	GENERATED_BODY()

	public:
	UBTService_UpdateLastKnownLocation();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
};