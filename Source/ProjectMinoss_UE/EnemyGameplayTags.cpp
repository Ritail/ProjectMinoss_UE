// EnemyGameplayTags.cpp
#include "EnemyGameplayTags.h"

namespace EnemyTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Patrol,      "AI.State.Patrol",      "Enemy is on patrol");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Chase,       "AI.State.Chase",       "Enemy is chasing target");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Search,      "AI.State.Search",      "Enemy is searching last-known-position");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Investigate, "AI.State.Investigate", "Enemy is investigating a noise");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Stunned,  "State.Debuff.Stunned",  "Enemy cannot act");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Blinded,  "State.Debuff.Blinded",  "Enemy cannot see");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Rage,          "State.Buff.Rage",          "Permanent rage: speed + sight boost");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_SoundOverload, "State.Buff.SoundOverload", "Temporary forced tracking via noise");
}