// BTDecorator_ZoneNotCompleted.cpp
#include "BTDecorator_ZoneNotCompleted.h"
#include "PatrolPoint.h"
#include "PuzzleZoneSubsystem.h"
#include "AIBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"

UBTDecorator_ZoneNotCompleted::UBTDecorator_ZoneNotCompleted()
{
	NodeName = TEXT("Zone Not Completed");
}

bool UBTDecorator_ZoneNotCompleted::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UWorld* World = OwnerComp.GetWorld();
	if (!World) return false;

	UPuzzleZoneSubsystem* Zones = World->GetSubsystem<UPuzzleZoneSubsystem>();
	if (!Zones) return false;

	FGameplayTag ZoneToCheck = ZoneIdOverride;
	if (!ZoneToCheck.IsValid())
	{
		// Fall back to CurrentPatrolPoint's ZoneId.
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		if (BB)
		{
			if (const APatrolPoint* Point = Cast<APatrolPoint>(BB->GetValueAsObject(BBKeys::CurrentPatrolPoint)))
			{
				ZoneToCheck = Point->ZoneId;
			}
		}
	}

	if (!ZoneToCheck.IsValid()) return true;  // unknown zone → permit (don't trap the enemy)
	return !Zones->IsZoneCompleted(ZoneToCheck);
}

FString UBTDecorator_ZoneNotCompleted::GetStaticDescription() const
{
	return ZoneIdOverride.IsValid()
		? FString::Printf(TEXT("Zone %s not completed"), *ZoneIdOverride.ToString())
		: TEXT("CurrentPatrolPoint zone not completed");
}