// AIBlackboardKeys.h
#pragma once

#include "CoreMinimal.h"

namespace BBKeys
{
	inline const FName TargetActor         = TEXT("TargetActor");
	inline const FName LastKnownLocation   = TEXT("LastKnownLocation");
	inline const FName InvestigateLocation = TEXT("InvestigateLocation");
	inline const FName CurrentPatrolPoint  = TEXT("CurrentPatrolPoint");
	inline const FName HomeLocation        = TEXT("HomeLocation");
	inline const FName StateTag            = TEXT("StateTag");
}

//Critical: these strings must match the Blackboard asset exactly. Case-sensitive. 
//If you rename in the BB editor, update here.