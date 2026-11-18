// EnemyGameplayTags.h
#pragma once

#include "NativeGameplayTags.h"

namespace EnemyTags
{
	// --- State tags (mirrored into Blackboard StateTag key) ---
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Patrol);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Chase);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Search);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Investigate);

	// --- Debuffs ---
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Stunned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Blinded);

	// --- Buffs ---
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Rage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_SoundOverload);
}