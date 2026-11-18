// PatrolRouteComponent.cpp
#include "PatrolRouteComponent.h"
#include "PatrolPoint.h"
#include "PuzzleZoneSubsystem.h"

UPatrolRouteComponent::UPatrolRouteComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

APatrolPoint* UPatrolRouteComponent::GetNextPoint(const UPuzzleZoneSubsystem* Zones)
{
    if (!Zones) return nullptr;

    // 1. Try to advance within the current major's minors.
    if (CurrentMajor && CurrentMajor->MinorPoints.Num() > 0)
    {
        if (APatrolPoint* Next = AdvanceMinor(Zones))
        {
            CurrentPoint = Next;
            return Next;
        }
    }

    // 2. Advance to the next valid major.
    if (APatrolPoint* NextMajor = AdvanceMajor(Zones))
    {
        CurrentMajor = NextMajor;
        CurrentMinorIdx = INDEX_NONE;

        // Take the first valid minor if the major has any.
        if (APatrolPoint* FirstMinor = AdvanceMinor(Zones))
        {
            CurrentPoint = FirstMinor;
            return FirstMinor;
        }
        CurrentPoint = NextMajor;
        return NextMajor;
    }

    // 3. No valid majors remain.
    return nullptr;
}

APatrolPoint* UPatrolRouteComponent::AdvanceMinor(const UPuzzleZoneSubsystem* Zones)
{
    if (!CurrentMajor) return nullptr;

    const int32 NumMinors = CurrentMajor->MinorPoints.Num();
    for (int32 Step = 1; Step <= NumMinors; ++Step)
    {
        const int32 Idx = CurrentMinorIdx + Step;
        if (Idx < 0 || Idx >= NumMinors) continue;
        APatrolPoint* Candidate = CurrentMajor->MinorPoints[Idx];
        if (Candidate && !Zones->IsZoneCompleted(Candidate->ZoneId))
        {
            CurrentMinorIdx = Idx;
            return Candidate;
        }
    }
    return nullptr;
}

APatrolPoint* UPatrolRouteComponent::AdvanceMajor(const UPuzzleZoneSubsystem* Zones)
{
    const int32 Num = Majors.Num();
    if (Num == 0) return nullptr;

    const int32 Start = (CurrentMajorIdx < 0) ? -1 : CurrentMajorIdx;
    for (int32 Step = 1; Step <= Num; ++Step)
    {
        const int32 Idx = (Start + Step + Num) % Num;
        APatrolPoint* Candidate = Majors[Idx];
        if (Candidate && !Zones->IsZoneCompleted(Candidate->ZoneId))
        {
            CurrentMajorIdx = Idx;
            return Candidate;
        }
    }
    return nullptr;
}

void UPatrolRouteComponent::ResetIndex()
{
    CurrentMajor = nullptr;
    CurrentPoint = nullptr;
    CurrentMajorIdx = INDEX_NONE;
    CurrentMinorIdx = INDEX_NONE;
}