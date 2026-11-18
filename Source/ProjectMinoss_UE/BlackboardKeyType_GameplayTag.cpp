// BlackboardKeyType_GameplayTag.cpp
#include "BlackboardKeyType_GameplayTag.h"
#include "BehaviorTree/BlackboardComponent.h"

namespace
{
    FORCEINLINE const FGameplayTag* TagFromMemory(const uint8* MemoryBlock)
    {
        return reinterpret_cast<const FGameplayTag*>(MemoryBlock);
    }
    FORCEINLINE FGameplayTag* TagFromMemory(uint8* MemoryBlock)
    {
        return reinterpret_cast<FGameplayTag*>(MemoryBlock);
    }
}

UBlackboardKeyType_GameplayTag::UBlackboardKeyType_GameplayTag(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 5.6: size is a constructor-set member, not a virtual GetSize() override.
    ValueSize = sizeof(FGameplayTag);
}

// --- Template static API ---
// KeyOb is unused in these bodies — the memory block already carries the data.
// The pointer is only there to satisfy the template's call signature.

FGameplayTag UBlackboardKeyType_GameplayTag::GetValue(const UBlackboardKeyType* /*KeyOb*/, const uint8* RawData)
{
    const FGameplayTag* Tag = TagFromMemory(RawData);
    return Tag ? *Tag : InvalidValue;
}

bool UBlackboardKeyType_GameplayTag::SetValue(UBlackboardKeyType* /*KeyOb*/, uint8* RawData, const FGameplayTag& Value)
{
    FGameplayTag* Tag = TagFromMemory(RawData);
    if (!Tag) return false;
    *Tag = Value;
    return true;
}

// --- Virtuals ---

void UBlackboardKeyType_GameplayTag::InitializeMemory(UBlackboardComponent& OwnerComp, uint8* MemoryBlock)
{
    new (MemoryBlock) FGameplayTag();
}

void UBlackboardKeyType_GameplayTag::FreeMemory(UBlackboardComponent& OwnerComp, uint8* MemoryBlock)
{
    TagFromMemory(MemoryBlock)->~FGameplayTag();
}

FString UBlackboardKeyType_GameplayTag::DescribeValue(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock) const
{
    const FGameplayTag* Tag = TagFromMemory(MemoryBlock);
    return (Tag && Tag->IsValid()) ? Tag->ToString() : TEXT("(None)");
}

bool UBlackboardKeyType_GameplayTag::TestBasicOperation(
    const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock, EBasicKeyOperation::Type Op) const
{
    const FGameplayTag* Tag = TagFromMemory(MemoryBlock);
    const bool bValid = Tag && Tag->IsValid();
    return (Op == EBasicKeyOperation::Set) ? bValid : !bValid;
}

EBlackboardCompare::Type UBlackboardKeyType_GameplayTag::CompareValues(
    const UBlackboardComponent& OwnerComp,
    const uint8* MemoryBlock,
    const UBlackboardKeyType* OtherKeyOb,
    const uint8* OtherMemoryBlock) const
{
    // Guard: a different key type means a different memory layout. The engine
    // never calls this with mismatched types, but a defensive check costs nothing
    // and prevents an out-of-bounds read if the contract ever changes.
    if (!OtherKeyOb || OtherKeyOb->GetValueSize() != GetValueSize())
    {
        return EBlackboardCompare::NotEqual;
    }

    const FGameplayTag* A = TagFromMemory(MemoryBlock);
    const FGameplayTag* B = TagFromMemory(OtherMemoryBlock);
    if (!A || !B) return EBlackboardCompare::NotEqual;
    return A->MatchesTagExact(*B) ? EBlackboardCompare::Equal : EBlackboardCompare::NotEqual;
}